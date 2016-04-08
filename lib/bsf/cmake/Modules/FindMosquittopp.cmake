# - Try to find Mosquittopp
# Once done, this will define
#
#  Mosquittopp_FOUND - system has Mosquittopp
#  Mosquittopp_INCLUDE_DIRS - the Mosquittopp include directories
#  Mosquittopp_LIBRARIES -link these to use Mosquittopp

include(LibFindMacros)

# Apparently this library does not require linking to mosquitto, but needs
# moquitto.h to work

# Dependencies
# libfind_package(Mosquittopp Mosquitto)  # version?

libfind_pkg_detect(Mosquittopp mosquittopp FIND_PATH mosquittopp.h mosquitto.h FIND_LIBRARY mosquittopp)

if (NOT Mosquittopp_VERSION)
  # TODO: find proper version string
  # set(Mosquittopp_VERSION 1.4.3)
endif()

libfind_process(Mosquittopp)
