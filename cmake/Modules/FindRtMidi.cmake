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

# TODO: fix non found version?

libfind_process(RtMidi)
