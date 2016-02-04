# - Try to find RtMidi
# Once done, this will define
#
#  RtMidi_FOUND - system has RtMidi
#  RtMidi_INCLUDE_DIRS - the RtMidi include directories
#  RtMidi_LIBRARIES -link these to use RtMidi

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(RtMidi_PKGCONF rtmidi)

# Include dir
find_path(RtMidi_INCLUDE_DIR
  NAMES RtMidi.h
  PATHS ${RtMidi_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(RtMidi_LIBRARY
  NAMES rtmidi
  PATHS ${RtMidi_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(RtMidi_PROCESS_INCLUDES RtMidi_INCLUDE_DIR)
set(RtMidi_PROCESS_LIBS RtMidi_LIBRARY)
libfind_process(RtMidi)
