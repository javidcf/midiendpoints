# - Try to find Mosquitto
# Once done, this will define
#
#  Mosquittopp_FOUND - system has Mosquitto
#  Mosquittopp_INCLUDE_DIRS - the Mosquitto include directories
#  Mosquittopp_LIBRARIES -link these to use Mosquitto

include(LibFindMacros)

libfind_pkg_detect(Mosquitto mosquitto FIND_PATH mosquitto.h FIND_LIBRARY mosquitto)

if (NOT Mosquitto_VERSION)
  # TODO: find proper version string
  # set(Mosquitto_VERSION 1.4.3)
endif()

libfind_process(Mosquitto)
