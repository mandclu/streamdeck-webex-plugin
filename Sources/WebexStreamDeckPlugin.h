// Martijn Smit <martijn@mandclu.org / @smitmartijn>
#include <StreamDeckSDK/ESDBasePlugin.h>

#include <mutex>
#include <set>

using json = nlohmann::json;

class CallBackTimer;

// OS specific commands
std::string osGetWebexStatus();
void osToggleWebexMute();
void osToggleWebexShare();
void osToggleWebexVideo();
void osLeaveWebexMeeting();
void osFocusWebexWindow();
void osToggleWebexRecordCloud();
void osToggleWebexRecordLocal();
void osMuteAll();
void osUnmuteAll();
void osWebexCustomShortcut(std::string shortcut);

class WebexStreamDeckPlugin : public ESDBasePlugin
{
public:
  WebexStreamDeckPlugin();
  virtual ~WebexStreamDeckPlugin();

  void KeyDownForAction(
      const std::string &inAction,
      const std::string &inContext,
      const json &inPayload,
      const std::string &inDeviceID) override;
  void KeyUpForAction(
      const std::string &inAction,
      const std::string &inContext,
      const json &inPayload,
      const std::string &inDeviceID) override;
  void WillAppearForAction(
      const std::string &inAction,
      const std::string &inContext,
      const json &inPayload,
      const std::string &inDeviceID) override;
  void WillDisappearForAction(
      const std::string &inAction,
      const std::string &inContext,
      const json &inPayload,
      const std::string &inDeviceID) override;

  void SendToPlugin(
      const std::string &inAction,
      const std::string &inContext,
      const json &inPayload,
      const std::string &inDeviceID) override;

  void DeviceDidConnect(const std::string &inDeviceID, const json &inDeviceInfo)
      override;
  void DeviceDidDisconnect(const std::string &inDeviceID) override;

  void DidReceiveGlobalSettings(const json &inPayload) override;
  void DidReceiveSettings(
      const std::string &inAction,
      const std::string &inContext,
      const json &inPayload,
      const std::string &inDeviceID) override;

private:
  void UpdateWebexStatus();

  std::recursive_mutex mVisibleContextsMutex;
  std::set<std::string> mVisibleContexts;

  // SD button array to remember the context ID
  struct Button
  {
    std::string action;
    std::string context;
  };

  void UpdateState(const std::string &context, const std::string &device = "");

  std::map<std::string, Button> mButtons;
  CallBackTimer *mTimer;
};
