
include (ExternalProject)

# librtmidi

# find_package (RtMidi QUIET)

if (NOT RtMidi_FOUND)
  ExternalProject_Add(
    RtMidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG 2.1.0
    CONFIGURE_COMMAND autoconf && ./configure --with-alsa --with-jack  # Linux only
    BUILD_COMMAND make
    BUILD_IN_SOURCE 1
  )
else ()
  add_custom_target (RtMidi)
endif ()

# Paho

# find_package (Paho QUIET)
#
# if (NOT Paho_FOUND)
#   find_package (OpenSSL REQUIRED)
#   ExternalProject_Add(
#     Paho
#     GIT_REPOSITORY https://git.eclipse.org/r/paho/org.eclipse.paho.mqtt.c
#     GIT_TAG v1.0.3
#     BUILD_COMMAND make
#     BUILD_IN_SOURCE 1
#   )
# else ()
#   add_custom_target (Paho)
# endif ()


# Paho-Cpp

#find_package (PahoCpp QUIET)
