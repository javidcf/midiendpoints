# - Try to find RtMidi
# Once done, this will define
#
#  RtMidi_FOUND - system has RtMidi
#  RtMidi_INCLUDE_DIRS - the RtMidi include directories
#  RtMidi_LIBRARIES -link these to use RtMidi

include(LibFindMacros)

# Dependencies
libfind_package(RtMidi Jack)  # version?

libfind_pkg_detect(RtMidi rtmidi FIND_PATH RtMidi.h FIND_LIBRARY rtmidi)

if (NOT RtMidi_VERSION)
  # TODO: find proper version string
  # set(RtMidi_VERSION 2.1.0)
endif()

libfind_process(RtMidi)
