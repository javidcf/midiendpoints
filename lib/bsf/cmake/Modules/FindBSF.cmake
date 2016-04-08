# - Try to find BSF
# Once done, this will define
#
#  BSF_FOUND - system has BSF
#  BSF_INCLUDE_DIRS - the BSF include directories
#  BSF_LIBRARIES -link these to use BSF

include(LibFindMacros)

# Dependencies
libfind_package(BSF Mosquittopp)  # version?

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(BSF_PKGCONF bsf)

# Include dir
find_path(BSF_INCLUDE_DIR
  NAMES bsf/Sensor.h
  NAMES bsf/SensorClient.h
  PATHS ${BSF_PKGCONF_INCLUDE_DIRS}
)

# No compiled library
set (BSF_LIBRARY "")

if (NOT BSF_VERSION)
  # TODO: find proper version string
  #set(BSF_VERSION 0.0)
endif()

libfind_process(BSF)
