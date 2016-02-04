# - Try to find Jack
# Once done, this will define
#
#  Jack_FOUND - system has Jack
#  Jack_INCLUDE_DIRS - the Jack include directories
#  Jack_LIBRARIES -link these to use Jack

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Jack_PKGCONF jack)

# Include dir
find_path(Jack_INCLUDE_DIR
  NAMES jack/jack.h
  PATHS ${Jack_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(Jack_LIBRARY
  NAMES jack
  PATHS ${Jack_PKGCONF_LIBRARY_DIRS}
)

libfind_process(Jack)
