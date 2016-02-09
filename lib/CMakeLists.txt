
include (ExternalProject)

# json
find_package (Json ${JSON_VERSION} QUIET)
if (NOT Json_FOUND)
  message (STATUS "Retrieving json library...")
  ExternalProject_Add (
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    # GIT_TAG v${JSON_VERSION}  # not existing yet
    GIT_TAG HEAD
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/json.install ${CMAKE_BINARY_DIR}/lib/json
  )
endif ()


# Bootstrap dependencies!
# Still not working

# include (ExternalProject)
#
#
# # RtMidi
# find_package (RtMidi ${RTMIDI_VERSION} QUIET)
#
# if (NOT RtMidi_FOUND)
#   set (RtMidi_INSTALL_DIR ${CMAKE_BINARY_DIR}/RtMidi)
#   file (MAKE_DIRECTORY
#     ${RtMidi_INSTALL_DIR}/lib
#     ${RtMidi_INSTALL_DIR}/pkgconfig
#     ${RtMidi_INSTALL_DIR}/include
#     ${RtMidi_INSTALL_DIR}/bin
#   )
#   find_package (Git)
#   ExternalProject_Add (
#     RtMidi
#     GIT_REPOSITORY https://github.com/thestk/rtmidi.git
#     GIT_TAG 2.1.0
#     # Linux only for now
#     CONFIGURE_COMMAND autoconf && ./configure --with-alsa --with-jack --prefix=${CMAKE_INSTALL_PREFIX}
#     #BUILD_COMMAND make
#     BUILD_IN_SOURCE 1
#     INSTALL_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/rtmidi_2.1.0.install ${RtMidi_INSTALL_DIR}
#   )
#   # TODO set vars
# else ()
#   add_custom_target (RtMidi)
# endif ()
