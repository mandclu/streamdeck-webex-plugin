// Martin Anderson-Clutz <mandclu@gmail.com / @mandclu>
#include "WebexStreamDeckPlugin.h"
#include <StreamDeckSDK/ESDLogger.h>

extern std::string m_webexMenuMeeting;
extern std::string m_webexMenuMuteAudio;
extern std::string m_webexMenuUnmuteAudio;

extern std::string m_webexMenuStartVideo;
extern std::string m_webexMenuStopVideo;

extern std::string m_webexMenuStartShare;
extern std::string m_webexMenuStopShare;

extern std::string m_webexMenuStartRecordToCloud;
extern std::string m_webexMenuStopRecordToCloud;

extern std::string m_webexMenuStartRecord;

extern std::string m_webexMenuStartRecordLocal;
extern std::string m_webexMenuStopRecordLocal;

extern std::string m_webexMenuWindow;
extern std::string m_webexMenuClose;

extern std::string m_webexMenuMuteAll;
extern std::string m_webexMenuUnmuteAll;

char *execAndReturn(const char *command)
{
  FILE *fp;
  char *line = NULL;
  // Following initialization is equivalent to char* result = ""; and just
  // initializes result to an empty string, only it works with
  // -Werror=write-strings and is so much less clear.
  char *result = (char *)calloc(1, 1);
  size_t len = 0;

  fflush(NULL);
  fp = popen(command, "r");
  if (fp == NULL)
  {
    printf("Cannot execute command:\n%s\n", command);
    ESDDebug("Cannot execute command:\n%s\n", command);
    return NULL;
  }

  while (getline(&line, &len, fp) != -1)
  {
    // +1 below to allow room for null terminator.
    result = (char *)realloc(result, strlen(result) + strlen(line) + 1);
    // +1 below so we copy the final null terminator.
    strncpy(result + strlen(result), line, strlen(line) + 1);
    free(line);
    line = NULL;
  }

  fflush(fp);
  if (pclose(fp) != 0)
  {
    perror("Cannot close stream.\n");
  }
  return result;
}

std::string osGetWebexStatus()
{
  /*
  Original AS:
  set webexStatus to "closed"
set muteStatus to "disabled"
set videoStatus to "disabled"
set shareStatus to "disabled"
set recordStatus to "disabled"
tell application "System Events"
	if exists (window 1 of process "Webex") then
		set webexStatus to "open"
		tell application process "Webex"
			if exists (menu bar item "Meeting" of menu bar 1) then
				set webexStatus to "call"
				if exists (menu item "Mute audio" of menu 1 of menu bar item "Meeting" of menu bar 1) then
					set muteStatus to "unmuted"
				else
					set muteStatus to "muted"
				end if
				if exists (menu item "Start Video" of menu 1 of menu bar item "Meeting" of menu bar 1) then
					set videoStatus to "stopped"
				else
					set videoStatus to "started"
				end if
				if exists (menu item "Start Share" of menu 1 of menu bar item "Meeting" of menu bar 1) then
					set shareStatus to "stopped"
				else
					set shareStatus to "started"
				end if
				if exists (menu item "Record to the Cloud" of menu 1 of menu bar item "Meeting" of menu bar 1) then
					set recordStatus to "stopped"
				else if exists (menu item "Record" of menu 1 of menu bar item "Meeting" of menu bar 1) then
					set recordStatus to "stopped"
				else
					set recordStatus to "started"
				end if
			end if
		end tell
	end if
end tell
do shell script "echo webexMute:" & (muteStatus as text) & ",webexVideo:" & (videoStatus as text) & ",webexStatus:" & (webexStatus as text) & ",webexShare:" & (shareStatus as text) & ",webexRecord:" & (recordStatus as text)
  */
  // ESDDebug("APPLESCRIPT_GET_STATUS: %s", APPLESCRIPT_GET_STATUS);
  const std::string appleScript = "set webexStatus to \"closed\"\n"
                                  "set muteStatus to \"disabled\"\n"
                                  "set videoStatus to \"disabled\"\n"
                                  "set shareStatus to \"disabled\"\n"
                                  "set recordStatus to \"disabled\"\n"
                                  "set speakerViewStatus to \"disabled\"\n"
                                  "set minimalView to \"disabled\"\n"
                                  "tell application \"System Events\"\n"
                                  "	if (get name of every application process) contains \"Webex\" then\n"
                                  "		set webexStatus to \"open\"\n"
                                  "		tell application process \"Webex\"\n"
                                  "			if exists (menu bar item \"" +
                                  m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                      "				set webexStatus to \"call\"\n"
                                                      "				if exists (menu item \"" +
                                  m_webexMenuMuteAudio + "\" of menu 1 of menu bar item \"" +
                                  m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                      "					set muteStatus to \"unmuted\"\n"
                                                      "				else\n"
                                                      "					set muteStatus to \"muted\"\n"
                                                      "				end if\n"
                                                      "				if exists (menu item \"" +
                                  m_webexMenuStartVideo + "\" of menu 1 of menu bar item \"" +
                                  m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                      "					set videoStatus to \"stopped\"\n"
                                                      "				else\n"
                                                      "					set videoStatus to \"started\"\n"
                                                      "				end if\n"
                                                      "				if exists (menu item \"" +
                                  m_webexMenuStartShare + "\" of menu 1 of menu bar item \"" +
                                  m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                      "					set shareStatus to \"stopped\"\n"
                                                      "				else\n"
                                                      "					set shareStatus to \"started\"\n"
                                                      "				end if\n"
                                                      "				if exists (menu item \"" +
                                  m_webexMenuStartRecordToCloud + "\" of menu 1 of menu bar item \"" +
                                  m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                      "					set recordStatus to \"stopped\"\n"
                                                      "				else if exists (menu item \"" +
                                  m_webexMenuStartRecord + "\" of menu 1 of menu bar item \"" +
                                  m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                      "					set recordStatus to \"stopped\"\n"
                                                      "				else\n"
                                                      "					set recordStatus to \"started\"\n"
                                                      "				end if\n"
                                                      "			end if\n"
                                                      "		end tell\n"
                                                      "	end if\n"
                                                      "end tell\n"
                                                      "do shell script \"echo webexMute:\" & (muteStatus as text) & \",webexVideo:\" & (videoStatus as text) & \",webexStatus:\" & (webexStatus as text) & \",webexShare:\" & (shareStatus as text) & \",webexRecord:\" & (recordStatus as text)";

  std::string cmd = "osascript -e '";
  cmd.append(appleScript);
  cmd.append("'");
  char *webexStatus = execAndReturn(cmd.c_str());

  return std::string(webexStatus);
}

void osToggleWebexMute()
{
  const std::string script = "osascript -e '"
                             "tell application \"Webex\"\n"
                             "  tell application \"System Events\" to tell application process \"Webex\"\n"
                             "    if exists (menu item \"" +
                             m_webexMenuUnmuteAudio + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                                                                              "      click (menu item \"" +
                             m_webexMenuUnmuteAudio + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                              "    else\n"
                                                                                                              "      click (menu item \"" +
                             m_webexMenuMuteAudio + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                            "    end if\n"
                                                                                                            "  end tell\n"
                                                                                                            "end tell'\n";
  //ESDDebug(script.c_str());
  system(script.c_str());
}

void osToggleWebexShare()
{
  const std::string script = "osascript -e '"
                             "tell application \"Webex\"\n"
                             "  tell application \"System Events\" to tell application process \"Webex\"\n"
                             "    if exists (menu item \"" +
                             m_webexMenuStartShare + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                                                                             "      click (menu item \"" +
                             m_webexMenuStartShare + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                             "    else\n"
                                                                                                             "      click (menu item \"" +
                             m_webexMenuStopShare + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                            "    end if\n"
                                                                                                            "  end tell\n"
                                                                                                            "end tell'\n";
  //ESDDebug(script.c_str());
  system(script.c_str());
}

void osToggleWebexVideo()
{
  const std::string script = "osascript -e '"
                             "tell application \"Webex\"\n"
                             "  tell application \"System Events\" to tell application process \"Webex\"\n"
                             "    if exists (menu item \"" +
                             m_webexMenuStartVideo + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                                                                             "      click (menu item \"" +
                             m_webexMenuStartVideo + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                             "    else\n"
                                                                                                             "      click (menu item \"" +
                             m_webexMenuStopVideo + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                            "    end if\n"
                                                                                                            "  end tell\n"
                                                                                                            "end tell'\n";
  //ESDDebug(script.c_str());
  system(script.c_str());
}

void osLeaveWebexMeeting()
{
  const std::string script = "osascript -e '"
                             "tell application \"Webex\" to activate\n"
                             "tell application \"System Events\" to tell application process \"Webex\"\n"
                             "	if exists (menu bar item \"" +
                             m_webexMenuWindow + "\" of menu bar 1) then\n"
                                                "		click (menu item \"" +
                             m_webexMenuClose + "\" of menu 1 of menu bar item \"" + m_webexMenuWindow + "\" of menu bar 1)\n"
                                                                                                       "		delay 0.5\n"
                                                                                                       "		click button 1 of window 1\n"
                                                                                                       "	end if\n"
                                                                                                       "end tell'";
  //ESDDebug(script.c_str());
  system(script.c_str());
}

void osFocusWebexWindow()
{
  const char *script = "osascript -e 'tell application \"Webex\"\nactivate\nend tell'";
  //ESDDebug(script);
  system(script);
}

void osToggleWebexRecordCloud()
{
  const std::string script = "osascript -e '"
                             "tell application \"Webex\"\n"
                             "  tell application \"System Events\" to tell application process \"Webex\"\n"
                             "    if exists (menu item \"" +
                             m_webexMenuStartRecordToCloud + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                                                                                     "      click (menu item \"" +
                             m_webexMenuStartRecordToCloud + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                                     "    else if exists (menu item \"" +
                             m_webexMenuStartRecord + "\" of menu 1 of menu bar item \"" +
                             m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                 "      click (menu item \"" +
                             m_webexMenuStartRecord + "\" of menu 1 of menu bar item \"" +
                             m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                 "    else\n"
                                                 "      click (menu item \"" +
                             m_webexMenuStopRecordToCloud + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                                    "    end if\n"
                                                                                                                    "  end tell\n"
                                                                                                                    "end tell'\n";
  //ESDDebug(script.c_str());
  system(script.c_str());
}

void osToggleWebexRecordLocal()
{
  const std::string script = "osascript -e '"
                             "tell application \"Webex\"\n"
                             "  tell application \"System Events\" to tell application process \"Webex\"\n"
                             "    if exists (menu item \"" +
                             m_webexMenuStartRecordLocal + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                                                                                   "      click (menu item \"" +
                             m_webexMenuStartRecordLocal + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                                   "    else if exists (menu item \"" +
                             m_webexMenuStartRecord + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                                                                              "      click (menu item \"" +
                             m_webexMenuStartRecordLocal + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                                   "    else\n"
                                                                                                                   "      click (menu item \"" +
                             m_webexMenuStopRecordLocal + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                                  "    end if\n"
                                                                                                                  "  end tell\n"
                                                                                                                  "end tell'\n";
  //ESDDebug(script.c_str());
  system(script.c_str());
}

void osMuteAll()
{
  const std::string script = "osascript -e '"
                             "tell application \"Webex\"\n"
                             "  tell application \"System Events\" to tell application process \"Webex\"\n"
                             "    if exists (menu item \"" +
                             m_webexMenuMuteAll + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                                                                          "      click (menu item \"" +
                             m_webexMenuMuteAll + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                          "      activate\n"
                                                                                                          "      set frontmost to true\n"
                                                                                                          "      delay 0.5\n"
                                                                                                          "      keystroke return\n"
                                                                                                          "    end if\n"
                                                                                                          "  end tell\n"
                                                                                                          "end tell'\n";
  //ESDDebug(script.c_str());
  system(script.c_str());
}
void osUnmuteAll()
{
  const std::string script = "osascript -e '"
                             "tell application \"Webex\"\n"
                             "  tell application \"System Events\" to tell application process \"Webex\"\n"
                             "    if exists (menu item \"" +
                             m_webexMenuUnmuteAll + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1) then\n"
                                                                                                            "      click (menu item \"" +
                             m_webexMenuUnmuteAll + "\" of menu 1 of menu bar item \"" + m_webexMenuMeeting + "\" of menu bar 1)\n"
                                                                                                            "    end if\n"
                                                                                                            "  end tell\n"
                                                                                                            "end tell'\n";
  //ESDDebug(script.c_str());
  system(script.c_str());
}


void osWebexCustomShortcut(std::string shortcut)
{
  // build the apple script based on the incoming shortcut. Modifiers should always be first, so first check for mod keys, then move on to the key
  std::string s = shortcut;
  std::string delimiter = "+";

  /*
    We want to build something like this:

    tell application "Webex" to activate
    tell application "Webex"
      tell application "System Events" to tell application process "Webex"
        keystroke "v" using {shift down, command down}
      end tell
    end tell
  */

  std::string as_modifiers = "";
  std::string as_key       = "";
  // "explode" the shortcut using '+' as the delimiter, by finding the first instance of the delimiter, substr()'ing that part and then deleting the first part and move on
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos)
  {
    token = s.substr(0, pos);
    //ESDDebug("Token: %s", token.c_str());
    //ESDDebug("s: %s", s.c_str());
    s.erase(0, pos + delimiter.length());
    //ESDDebug("s: %s", s.c_str());


    if(token == "shift") {
      as_modifiers += "shift down, ";
    }
    else if(token == "command") {
      as_modifiers += "command down, ";
    }
    else if(token == "control") {
      as_modifiers += "control down, ";
    }
    else if(token == "option") {
      as_modifiers += "option down, ";
    }
    else {
      // regular key!
      // we're assuming that nothing comes after the key, which it shouldn't, so we might as well break
      break;
    }
  }

  // the leftover will be the key itself
  as_key = s;
  // convert string to upper case
  std::for_each(as_key.begin(), as_key.end(), [](char & c){
    c = ::tolower(c);
  });

  // remove the last ", " from the modifiers string
  as_modifiers = as_modifiers.substr(0, as_modifiers.size()-2);

  const std::string script = "osascript -e '"
                       "tell application \"Webex\" to activate\n"
                       "tell application \"Webex\"\n"
                       "  tell application \"System Events\" to tell application process \"Webex\"\n"
                       "    keystroke \""+as_key+"\" using {"+as_modifiers+"}\n"
                       "  end tell\n"
                       "end tell'";
  //ESDDebug(script.c_str());
  system(script.c_str());
}
