# - Try to find PahoCpp
# Once done, this will define
#
#  PahoCpp_FOUND - system has PahoCpp
#  PahoCpp_INCLUDE_DIRS - the PahoCpp include directories
#  PahoCpp_LIBRARIES -link these to use PahoCpp

include(LibFindMacros)

# Dependencies
libfind_package(PahoCpp Paho)  # version?

libfind_pkg_detect(PahoCpp pahopp FIND_PATH mqtt/client.h FIND_LIBRARY mqttpp)

if (NOT PahoCpp_VERSION)
  # TODO: find proper version string
  # set(PahoCpp_VERSION 0.1)
endif()

libfind_process(PahoCpp)
