{
  "Actions": [
    {
      "States": [
        {
          "Image": "streamdeck-webex-muted"
        },
        {
          "Image": "streamdeck-webex-unmuted"
        },
        {
          "Image": "streamdeck-webex-muted-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-webex-muted-actionicon",
      "Name": "Mute Toggle",
      "Tooltip": "Toggle Webex Mute Status",
      "UUID": "com.mandclu.webex.mutetoggle",
      "PropertyInspectorPath": "propertyinspector/index-mute.html"
    },
    {
      "States": [
        {
          "Image": "streamdeck-webex-video-stopped"
        },
        {
          "Image": "streamdeck-webex-video-started"
        },
        {
          "Image": "streamdeck-webex-video-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-webex-video-actionicon",
      "Name": "Video Toggle",
      "Tooltip": "Toggle Webex Video",
      "UUID": "com.mandclu.webex.videotoggle",
      "PropertyInspectorPath": "propertyinspector/index-video.html"
    },
    {
      "States": [
        {
          "Image": "streamdeck-webex-share-start"
        },
        {
          "Image": "streamdeck-webex-share-stop"
        },
        {
          "Image": "streamdeck-webex-share-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-webex-share-actionicon",
      "Name": "Share Toggle",
      "Tooltip": "Bring up the share screen window, or stop sharing",
      "UUID": "com.mandclu.webex.sharetoggle",
      "PropertyInspectorPath": "propertyinspector/index-share.html"
    },
    {
      "States": [
        {
          "Image": "streamdeck-webex-focus"
        },
        {
          "Image": "streamdeck-webex-focus-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-webex-focus-actionicon",
      "Name": "Focus",
      "Tooltip": "Bring the Webex window to the front",
      "UUID": "com.mandclu.webex.focus",
      "PropertyInspectorPath": "propertyinspector/index.html"
    },
    {
      "States": [
        {
          "Image": "streamdeck-webex-leave"
        },
        {
          "Image": "streamdeck-webex-leave-closed"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-webex-leave-actionicon",
      "Name": "Leave Meeting",
      "Tooltip": "Leave an active meeting. If you're the host, this ends the meeting.",
      "UUID": "com.mandclu.webex.leave",
      "PropertyInspectorPath": "propertyinspector/index-leave.html"
    },
    {
      "States": [
        {
          "Image": "streamdeck-webex-record-cloud-stopped"
        },
        {
          "Image": "streamdeck-webex-record-cloud-started"
        },
        {
          "Image": "streamdeck-webex-record-cloud-disabled"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-webex-record-cloud-actionicon",
      "Name": "Cloud Record Toggle",
      "Tooltip": "Toggle Webex Recording to the Cloud",
      "UUID": "com.mandclu.webex.recordcloudtoggle",
      "PropertyInspectorPath": "propertyinspector/index-record-cloud.html"
    },
    {
      "States": [
        {
          "Image": "streamdeck-webex-record-local-stopped"
        },
        {
          "Image": "streamdeck-webex-record-local-started"
        },
        {
          "Image": "streamdeck-webex-record-local-disabled"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-webex-record-local-actionicon",
      "Name": "Local Record Toggle",
      "Tooltip": "Toggle Webex Recording to your local computer",
      "UUID": "com.mandclu.webex.recordlocaltoggle",
      "PropertyInspectorPath": "propertyinspector/index-record-local.html"
    },
    {
      "States": [
        {
          "Image": "streamdeck-webex-unmuteall"
        },
        {
          "Image": "streamdeck-webex-unmuteall-disabled"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-webex-unmuteall-actionicon",
      "Name": "Ask All to Unmute",
      "Tooltip": "Ask all participants to unmute",
      "UUID": "com.mandclu.webex.unmuteall",
      "PropertyInspectorPath": "propertyinspector/index-unmute-all.html"
    },
    {
      "States": [
        {
          "Image": "streamdeck-webex-muteall"
        },
        {
          "Image": "streamdeck-webex-muteall-disabled"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-webex-muteall-actionicon",
      "Name": "Mute All Participants",
      "Tooltip": "Mute All Participants",
      "UUID": "com.mandclu.webex.muteall",
      "PropertyInspectorPath": "propertyinspector/index-mute-all.html"
    },
    {
      "States": [
        {
          "Image": "streamdeck-webex-customshortcut"
        },
        {
          "Image": "streamdeck-webex-customshortcut-disabled"
        }
      ],
      "SupportedInMultiActions": true,
      "Icon": "streamdeck-webex-customshortcut-actionicon",
      "Name": "Custom Shortcut",
      "Tooltip": "Add a custom shortcut to do anything that Webex supports",
      "UUID": "com.mandclu.webex.customshortcut",
      "PropertyInspectorPath": "propertyinspector/index-customshortcut.html"
    }
  ],
  "CodePath": "sdwebexplugin.exe",
  "CodePathMac": "sdwebexplugin",
  "Author": "Martijn Smit",
  "Description": "Control your Webex meetings; easily mute yourself, start your video, share, record, quickly leave the meeting, and more.",
  "URL": "https://mandclu.org/stream-deck-plugin-for-webex/",
  "Name": "Webex Plugin",
  "Category": "Webex",
  "CategoryIcon": "video-camera",
  "Icon": "video-camera-plugin",
  "Version": "${CMAKE_PROJECT_VERSION}",
  "OS": [
    {
      "Platform": "mac",
      "MinimumVersion": "10.13"
    },
    {
      "Platform": "windows",
      "MinimumVersion" : "10"
    }
  ],
  "SDKVersion": 2,
  "Software": {
    "MinimumVersion": "4.1"
  }
}
