#include <StreamDeckSDK/ESDLogger.h>
#include <StreamDeckSDK/ESDMain.h>

#include "WebexStreamDeckPlugin.h"

int main(int argc, const char** argv) {
  ESDLogger::Get()->SetWin32DebugPrefix("[SDWebex] ");
  return esd_main(argc, argv, new WebexStreamDeckPlugin());
}
