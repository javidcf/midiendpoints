# - Try to find PahoCpp
# Once done, this will define
#
#  PahoCpp_FOUND - system has PahoCpp
#  PahoCpp_INCLUDE_DIRS - the PahoCpp include directories
#  PahoCpp_LIBRARIES -link these to use PahoCpp

include(LibFindMacros)

# Dependencies
libfind_package(PahoCpp Paho)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(PahoCpp_PKGCONF PahoCpp)

# Include dir
find_path(PahoCpp_INCLUDE_DIR
  NAMES mqtt/client.h   # This should be enough right?
  PATHS ${PahoCpp_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(PahoCpp_LIBRARY
  NAMES mqttpp
  PATHS ${PahoCpp_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(PahoCpp_PROCESS_INCLUDES PahoCpp_INCLUDE_DIR)
set(PahoCpp_PROCESS_LIBS PahoCpp_LIBRARY)
libfind_process(PahoCpp)
