// Martijn Smit <martijn@mandclu.org / @smitmartijn>
#include "WebexStreamDeckPlugin.h"

#include <StreamDeckSDK/EPLJSONUtils.h>
#include <StreamDeckSDK/ESDConnectionManager.h>
#include <StreamDeckSDK/ESDLogger.h>

#include <atomic>
#include <iostream>
#include <mutex>
#include <vector>

#define MUTETOGGLE_ACTION_ID "com.mandclu.webex.mutetoggle"
#define VIDEOTOGGLE_ACTION_ID "com.mandclu.webex.videotoggle"
#define SHARETOGGLE_ACTION_ID "com.mandclu.webex.sharetoggle"
#define FOCUS_ACTION_ID "com.mandclu.webex.focus"
#define LEAVE_ACTION_ID "com.mandclu.webex.leave"
#define RECORDCLOUDTOGGLE_ACTION_ID "com.mandclu.webex.recordcloudtoggle"
#define RECORDLOCALTOGGLE_ACTION_ID "com.mandclu.webex.recordlocaltoggle"
#define MUTEALL_ACTION_ID "com.mandclu.webex.muteall"
#define UNMUTEALL_ACTION_ID "com.mandclu.webex.unmuteall"
#define CUSTOMSHORTCUT_ACTION_ID "com.mandclu.webex.customshortcut"

std::string m_webexMenuMeeting = "Participant";
std::string m_webexMenuMuteAudio = "Mute Me";
std::string m_webexMenuUnmuteAudio = "Unmute Me";

std::string m_webexMenuStartVideo = "Start Video";
std::string m_webexMenuStopVideo = "Stop Video";

std::string m_webexMenuStartShare = "Start Share";
std::string m_webexMenuStopShare = "Stop Share";

std::string m_webexMenuStartRecordToCloud = "Record to the Cloud";
std::string m_webexMenuStopRecordToCloud = "Stop Recording";
std::string m_webexMenuStartRecord = "Record";
std::string m_webexMenuStartRecordLocal = "Record on this Computer";
std::string m_webexMenuStopRecordLocal = "Stop Recording";

std::string m_webexMenuWindow = "Window";
std::string m_webexMenuClose = "Close";

std::string m_webexMenuMuteAll = "Mute All";
std::string m_webexMenuUnmuteAll = "Unmute All";

class CallBackTimer
{
public:
  CallBackTimer() : _execute(false)
  {
  }
  ~CallBackTimer()
  {
    if (_execute.load(std::memory_order_acquire))
    {
      stop();
    };
  }
  void stop()
  {
    _execute.store(false, std::memory_order_release);
    if (_thd.joinable())
      _thd.join();
  }
  void start(int interval, std::function<void(void)> func)
  {
    if (_execute.load(std::memory_order_acquire))
    {
      stop();
    };
    _execute.store(true, std::memory_order_release);
    _thd = std::thread([this, interval, func]() {
      while (_execute.load(std::memory_order_acquire))
      {
        func();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
      }
    });
  }
  bool is_running() const noexcept
  {
    return (_execute.load(std::memory_order_acquire) && _thd.joinable());
  }

private:
  std::atomic<bool> _execute;
  std::thread _thd;
};

json getWebexStatus()
{
  // get Webex Mute status
  std::string status = osGetWebexStatus();
  //ESDDebug("OS script output status - %s", webexStatus);

  std::string statusMute;
  std::string statusVideo;
  std::string statusShare;
  std::string statusWebex;
  std::string statusRecord;
  std::string statusMuteAll = "enabled";
  std::string statusUnmuteAll = "enabled";

  if (status.find("webexStatus:open") != std::string::npos)
  {
    //ESDDebug("Webex Open!");
    statusWebex = "open";
  }
  else if (status.find("webexStatus:call") != std::string::npos)
  {
    //ESDDebug("Webex Call!");
    statusWebex = "call";
  }
  else
  {
    //ESDDebug("Webex Closed!");
    statusWebex = "closed";
  }

  // set mute, video, and sharing to disabled when there's no call
  if (statusWebex != "call")
  {
    //ESDDebug("Webex closed!");
    statusMute = "disabled";
    statusVideo = "disabled";
    statusShare = "disabled";
    statusRecord = "disabled";
    statusMuteAll = "disabled";
    statusUnmuteAll = "disabled";
  }
  else
  {
    // if there is a call, determine the mute, video, and share status
    if (status.find("webexMute:muted") != std::string::npos)
    {
      //ESDDebug("Webex Muted!");
      statusMute = "muted";
    }
    else if (status.find("webexMute:unmuted") != std::string::npos)
    {
      //ESDDebug("Webex Unmuted!");
      statusMute = "unmuted";
    }

    if (status.find("webexVideo:started") != std::string::npos)
    {
      //ESDDebug("Webex Video Started!");
      statusVideo = "started";
    }
    else if (status.find("webexVideo:stopped") != std::string::npos)
    {
      //ESDDebug("Webex Video Stopped!");
      statusVideo = "stopped";
    }

    if (status.find("webexShare:started") != std::string::npos)
    {
      //ESDDebug("Webex Screen Sharing Started!");
      statusShare = "started";
    }
    else if (status.find("webexShare:stopped") != std::string::npos)
    {
      //ESDDebug("Webex Screen Sharing Stopped!");
      statusShare = "stopped";
    }

    if (status.find("webexRecord:started") != std::string::npos)
    {
      //ESDDebug("Webex Record Started!");
      statusRecord = "started";
    }
    else if (status.find("webexRecord:stopped") != std::string::npos)
    {
      //ESDDebug("Webex Record Stopped!");
      statusRecord = "stopped";
    }
  }

  //ESDDebug("Webex status: %s", status.c_str());

  return json({{"statusWebex", statusWebex},
               {"statusMute", statusMute},
               {"statusVideo", statusVideo},
               {"statusRecord", statusRecord},
               {"statusShare", statusShare},
               {"statusMuteAll", statusMuteAll},
               {"statusUnmuteAll", statusUnmuteAll}});
}

WebexStreamDeckPlugin::WebexStreamDeckPlugin()
{
  ESDDebug("stored handle");

  // start a timer that updates the current status every 3 seconds
  mTimer = new CallBackTimer();
  mTimer->start(1500, [this]() { this->UpdateWebexStatus(); });
}

WebexStreamDeckPlugin::~WebexStreamDeckPlugin()
{
  ESDDebug("plugin destructor");
}

void WebexStreamDeckPlugin::UpdateWebexStatus()
{
  // This is running in a different thread
  if (mConnectionManager != nullptr)
  {
    std::scoped_lock lock(mVisibleContextsMutex);

    //ESDDebug("UpdateWebexStatus");
    // get webex status for mute, video and whether it's open
    json newStatus = getWebexStatus();
    //ESDDebug("CURRENT: Webex status %s", newStatus.dump().c_str());
    // Status images: 0 = active, 1 = cross, 2 = disabled
    auto newMuteState = 2;
    auto newVideoState = 2;
    auto newShareState = 2;
    auto newLeaveState = 1;
    auto newRecordState = 2;
    auto newFocusState = 1;
    auto newMuteAllState = 1;
    auto newUnmuteAllState = 1;

    // set mute, video, sharing, and focus to disabled when Webex is closed
    if (EPLJSONUtils::GetStringByName(newStatus, "statusWebex") == "closed")
    {
      newMuteState = 2;
      newVideoState = 2;
      newShareState = 2;
      newLeaveState = 1;
      newFocusState = 1;
      newRecordState = 2;
      newMuteAllState = 1;
      newUnmuteAllState = 1;
    }
    else if (EPLJSONUtils::GetStringByName(newStatus, "statusWebex") == "open")
    {
      // set mute, video, and sharing to disabled and focus to enabled when there's no call
      newFocusState = 0;
    }
    else
    {
      // if there is a call, determine the mute, video, and share status and enable both focus and leave

      if (EPLJSONUtils::GetStringByName(newStatus, "statusMute") == "muted")
      {
        //ESDDebug("CURRENT: Webex muted");
        newMuteState = 0;
      }
      else if (EPLJSONUtils::GetStringByName(newStatus, "statusMute") == "unmuted")
      {
        //ESDDebug("CURRENT: Webex unmuted");
        newMuteState = 1;
      }

      if (EPLJSONUtils::GetStringByName(newStatus, "statusVideo") == "stopped")
      {
        //ESDDebug("CURRENT: Webex video stopped");
        newVideoState = 0;
      }
      else if (EPLJSONUtils::GetStringByName(newStatus, "statusVideo") == "started")
      {
        //ESDDebug("CURRENT: Webex video started");
        newVideoState = 1;
      }

      if (EPLJSONUtils::GetStringByName(newStatus, "statusShare") == "stopped")
      {
        newShareState = 0;
      }
      else if (EPLJSONUtils::GetStringByName(newStatus, "statusShare") == "started")
      {
        newShareState = 1;
      }
      if (EPLJSONUtils::GetStringByName(newStatus, "statusRecord") == "stopped")
      {
        //ESDDebug("CURRENT: Webex record stopped");
        newRecordState = 0;
      }
      else if (EPLJSONUtils::GetStringByName(newStatus, "statusRecord") == "started")
      {
        //ESDDebug("CURRENT: Webex record started");
        newRecordState = 1;
      }

      // in a call, always have leave, focus, mute all and unmute all enabled
      newLeaveState = 0;
      newFocusState = 0;
      newMuteAllState = 0;
      newUnmuteAllState = 0;
    }

    // sanity check - is the button added?
    if (mButtons.count(MUTETOGGLE_ACTION_ID))
    {
      // update mute button
      const auto button = mButtons[MUTETOGGLE_ACTION_ID];
      // ESDDebug("Mute button context: %s", button.context.c_str());
      mConnectionManager->SetState(newMuteState, button.context);
    }

    // sanity check - is the button added?
    if (mButtons.count(VIDEOTOGGLE_ACTION_ID))
    {
      // update video button
      const auto button = mButtons[VIDEOTOGGLE_ACTION_ID];
      // ESDDebug("Video button context: %s", button.context.c_str());
      mConnectionManager->SetState(newVideoState, button.context);
    }

    // sanity check - is the button added?
    if (mButtons.count(SHARETOGGLE_ACTION_ID))
    {
      // update video button
      const auto button = mButtons[SHARETOGGLE_ACTION_ID];
      // ESDDebug("Video button context: %s", button.context.c_str());
      mConnectionManager->SetState(newShareState, button.context);
    }

    // sanity check - is the button added?
    if (mButtons.count(LEAVE_ACTION_ID))
    {
      // update leave button
      const auto button = mButtons[LEAVE_ACTION_ID];
      // ESDDebug("Leave button context: %s", button.context.c_str());
      mConnectionManager->SetState(newLeaveState, button.context);
    }

    // sanity check - is the button added?
    if (mButtons.count(FOCUS_ACTION_ID))
    {
      // update focus button
      const auto button = mButtons[FOCUS_ACTION_ID];
      // ESDDebug("Focus button context: %s", button.context.c_str());
      mConnectionManager->SetState(newFocusState, button.context);
    }

    // sanity check - is the button added?
    if (mButtons.count(RECORDLOCALTOGGLE_ACTION_ID))
    {
      // update record button
      const auto button = mButtons[RECORDLOCALTOGGLE_ACTION_ID];
      // ESDDebug("Record button context: %s", button.context.c_str());
      mConnectionManager->SetState(newRecordState, button.context);
    }
    // sanity check - is the button added?
    if (mButtons.count(RECORDCLOUDTOGGLE_ACTION_ID))
    {
      // update record button
      const auto button = mButtons[RECORDCLOUDTOGGLE_ACTION_ID];
      // ESDDebug("Record button context: %s", button.context.c_str());
      mConnectionManager->SetState(newRecordState, button.context);
    }

    // sanity check - is the button added?
    if (mButtons.count(MUTEALL_ACTION_ID))
    {
      // update mute all button
      const auto button = mButtons[MUTEALL_ACTION_ID];
      // ESDDebug("Record button context: %s", button.context.c_str());
      mConnectionManager->SetState(newMuteAllState, button.context);
    }

    // sanity check - is the button added?
    if (mButtons.count(UNMUTEALL_ACTION_ID))
    {
      // update unmute all button
      const auto button = mButtons[UNMUTEALL_ACTION_ID];
      // ESDDebug("Record button context: %s", button.context.c_str());
      mConnectionManager->SetState(newUnmuteAllState, button.context);
    }

    // sanity check - is the button added?
    if (mButtons.count(CUSTOMSHORTCUT_ACTION_ID))
    {
      // update unmute all button
      const auto button = mButtons[CUSTOMSHORTCUT_ACTION_ID];
      // ESDDebug("Record button context: %s", button.context.c_str());
      mConnectionManager->SetState(newUnmuteAllState, button.context);
    }
  }
}

void WebexStreamDeckPlugin::KeyDownForAction(
    const std::string &inAction,
    const std::string &inContext,
    const json &inPayload,
    const std::string &inDeviceID)
{
  const auto state = EPLJSONUtils::GetIntByName(inPayload, "state");
}

void WebexStreamDeckPlugin::KeyUpForAction(
    const std::string &inAction,
    const std::string &inContext,
    const json &inPayload,
    const std::string &inDeviceID)
{
  ESDDebug("Key Up: %s", inPayload.dump().c_str());
  std::scoped_lock lock(mVisibleContextsMutex);

  json jsonSettings;
  EPLJSONUtils::GetObjectByName(inPayload, "settings", jsonSettings);

  const auto state = EPLJSONUtils::GetIntByName(inPayload, "state");
  bool updateStatus = false;
  auto newState = 0;

  if (inAction == MUTETOGGLE_ACTION_ID)
  {
    std::string webexMenuMeeting = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuMeeting");
    std::string webexMenuMuteAudio = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuMuteAudio");
    std::string webexMenuUnmuteAudio = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuUnmuteAudio");

    if (!webexMenuMeeting.empty())
      m_webexMenuMeeting = webexMenuMeeting;

    if (!webexMenuMuteAudio.empty())
      m_webexMenuMuteAudio = webexMenuMuteAudio;

    if (!webexMenuUnmuteAudio.empty())
      m_webexMenuUnmuteAudio = webexMenuUnmuteAudio;

    // state == 0 == want to be muted
    if (state != 0)
    {
      ESDDebug("Unmuting Webex!");
    }
    // state == 1 == want to be unmuted
    else
    {
      ESDDebug("Muting Webex!");
    }

    osToggleWebexMute();
    updateStatus = true;
  }
  else if (inAction == SHARETOGGLE_ACTION_ID)
  {
    std::string webexMenuMeeting = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuMeeting");
    std::string webexMenuStartShare = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuStartShare");
    std::string webexMenuStopShare = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuStopShare");

    if (!webexMenuMeeting.empty())
      m_webexMenuMeeting = webexMenuMeeting;

    if (!webexMenuStartShare.empty())
      m_webexMenuStartShare = webexMenuStartShare;

    if (!webexMenuStopShare.empty())
      m_webexMenuStopShare = webexMenuStopShare;

    // state == 0 == want to share
    if (state != 0)
    {
      ESDDebug("Sharing Screen on Webex!");
    }
    // state == 1 == want to stop sharing
    else
    {
      ESDDebug("Stopping Screen Sharing on Webex!");
    }

    osToggleWebexShare();
    updateStatus = true;
  }
  else if (inAction == VIDEOTOGGLE_ACTION_ID)
  {
    std::string webexMenuMeeting = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuMeeting");
    std::string webexMenuStartVideo = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuStartVideo");
    std::string webexMenuStopVideo = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuStopVideo");

    if (!webexMenuMeeting.empty())
      m_webexMenuMeeting = webexMenuMeeting;

    if (!webexMenuStartVideo.empty())
      m_webexMenuStartVideo = webexMenuStartVideo;

    if (!webexMenuStopVideo.empty())
      m_webexMenuStopVideo = webexMenuStopVideo;

    // state == 0 == want to be with video on
    if (state != 0)
    {
      ESDDebug("Starting Webex Video!");
    }
    // state == 1 == want to be with video off
    else
    {
      ESDDebug("Stopping Webex Video!");
    }

    osToggleWebexVideo();
    updateStatus = true;
  }
  // focus on Webex window
  else if (inAction == FOCUS_ACTION_ID)
  {
    ESDDebug("Focusing Webex window!");
    osFocusWebexWindow();
  }
  // leave Webex meeting, or end the meeting. When ending, this also clicks "End
  // for all"
  else if (inAction == LEAVE_ACTION_ID)
  {
    std::string webexMenuWindow = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuWindow");
    std::string webexMenuClose = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuClose");

    if (!webexMenuWindow.empty())
      m_webexMenuWindow = webexMenuWindow;

    if (!webexMenuClose.empty())
      m_webexMenuClose = webexMenuClose;

    ESDDebug("Leaving Webex meeting!");
    osLeaveWebexMeeting();
  }

  // toggles cloud recording
  else if (inAction == RECORDCLOUDTOGGLE_ACTION_ID)
  {
    std::string webexMenuMeeting = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuMeeting");
    std::string webexMenuStartRecordToCloud = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuStartRecordToCloud");
    std::string webexMenuStopRecordToCloud = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuStopRecordToCloud");
    std::string webexMenuStartRecord = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuStartRecord");

    if (!webexMenuMeeting.empty())
      m_webexMenuMeeting = webexMenuMeeting;

    if (!webexMenuStartRecordToCloud.empty())
      m_webexMenuStartRecordToCloud = webexMenuStartRecordToCloud;

    if (!webexMenuStopRecordToCloud.empty())
      m_webexMenuStopRecordToCloud = webexMenuStopRecordToCloud;

    if (!webexMenuStartRecord.empty())
      m_webexMenuStartRecord = webexMenuStartRecord;

    ESDDebug("Toggling Recording to the Cloud");
    osToggleWebexRecordCloud();
  }

  // toggles local recording
  else if (inAction == RECORDLOCALTOGGLE_ACTION_ID)
  {
    std::string webexMenuMeeting = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuMeeting");
    std::string webexMenuStartRecordLocal = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuStartRecordLocal");
    std::string webexMenuStopRecordLocal = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuStopRecordLocal");
    std::string webexMenuStartRecord = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuStartRecord");

    if (!webexMenuMeeting.empty())
      m_webexMenuMeeting = webexMenuMeeting;

    if (!webexMenuStartRecordLocal.empty())
      m_webexMenuStartRecordLocal = webexMenuStartRecordLocal;

    if (!webexMenuStopRecordLocal.empty())
      m_webexMenuStopRecordLocal = webexMenuStopRecordLocal;

    if (!webexMenuStartRecord.empty())
      m_webexMenuStartRecord = webexMenuStartRecord;

    ESDDebug("Toggling Recording Locally");
    osToggleWebexRecordLocal();
  }

  // muting all partitipants in a group meeting
  else if (inAction == MUTEALL_ACTION_ID)
  {
    std::string webexMenuMeeting = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuMeeting");
    std::string webexMenuMuteAll = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuMuteAll");

    if (!webexMenuMeeting.empty())
      m_webexMenuMeeting = webexMenuMeeting;

    if (!webexMenuMuteAll.empty())
      m_webexMenuMuteAll = webexMenuMuteAll;

    ESDDebug("Muting all Participants");
    osMuteAll();
  }

  // toggles local recording
  else if (inAction == UNMUTEALL_ACTION_ID)
  {
    std::string webexMenuMeeting = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuMeeting");
    std::string webexMenuUnmuteAll = EPLJSONUtils::GetStringByName(jsonSettings, "webexMenuUnmuteAll");

    if (!webexMenuMeeting.empty())
      m_webexMenuMeeting = webexMenuMeeting;

    if (!webexMenuUnmuteAll.empty())
      m_webexMenuUnmuteAll = webexMenuUnmuteAll;

    ESDDebug("Asking all Participants to Unmute");
    osUnmuteAll();
  }

  else if(inAction == CUSTOMSHORTCUT_ACTION_ID)
  {
    std::string webexCustomShortcut = EPLJSONUtils::GetStringByName(jsonSettings, "webexCustomShortcut");

    // sanity check
    if(!webexCustomShortcut.empty()) {
      osWebexCustomShortcut(webexCustomShortcut);
    }
  }

  if (updateStatus)
  {
    UpdateWebexStatus();
  }
}

void WebexStreamDeckPlugin::WillAppearForAction(
    const std::string &inAction,
    const std::string &inContext,
    const json &inPayload,
    const std::string &inDeviceID)
{
  std::scoped_lock lock(mVisibleContextsMutex);
  // Remember the button context for the timer updates
  mVisibleContexts.insert(inContext);
  // ESDDebug("Will appear: %s %s", inAction, inContext);
  mButtons[inAction] = {inAction, inContext};
}

void WebexStreamDeckPlugin::WillDisappearForAction(
    const std::string &inAction,
    const std::string &inContext,
    const json &inPayload,
    const std::string &inDeviceID)
{
  // Remove the context
  std::scoped_lock lock(mVisibleContextsMutex);
  mVisibleContexts.erase(inContext);
  mButtons.erase(inAction);
}

void WebexStreamDeckPlugin::SendToPlugin(
    const std::string &inAction,
    const std::string &inContext,
    const json &inPayload,
    const std::string &inDeviceID)
{
  // Nothing to do
}

void WebexStreamDeckPlugin::DeviceDidConnect(
    const std::string &inDeviceID,
    const json &inDeviceInfo)
{
  // Nothing to do
}

void WebexStreamDeckPlugin::DeviceDidDisconnect(const std::string &inDeviceID)
{
  // Nothing to do
}

void WebexStreamDeckPlugin::DidReceiveGlobalSettings(const json &inPayload)
{
  ESDDebug("DidReceiveGlobalSettings");
  ESDDebug(EPLJSONUtils::GetString(inPayload).c_str());
}

void WebexStreamDeckPlugin::DidReceiveSettings(
    const std::string &inAction,
    const std::string &inContext,
    const json &inPayload,
    const std::string &inDeviceID)
{
  ESDDebug("DidReceiveSettings");
  ESDDebug(EPLJSONUtils::GetString(inPayload).c_str());

  WillAppearForAction(inAction, inContext, inPayload, inDeviceID);
}
