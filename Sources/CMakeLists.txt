set(CMAKE_CXX_STANDARD 20)

set(
  SOURCES
  WebexStreamDeckPlugin.cpp
  main.cpp
)

if(WIN32)
  set(SOURCES
    ${SOURCES}
    Windows.cpp
  )
endif()

if(APPLE)
  set(SOURCES
    ${SOURCES}
    MacOS.cpp
  )
endif()



add_executable(
  sdwebexplugin
  ${SOURCES}
)
target_link_libraries(sdwebexplugin json StreamDeckSDK)
install(TARGETS sdwebexplugin DESTINATION ${CMAKE_INSTALL_PREFIX})
