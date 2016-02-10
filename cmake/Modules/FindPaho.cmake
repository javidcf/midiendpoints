# - Try to find Paho
# Once done, this will define
#
#  Paho_FOUND - system has Paho
#  Paho_INCLUDE_DIRS - the Paho include directories
#  Paho_LIBRARIES -link these to use Paho

include(LibFindMacros)

# Which one? paho-mqtt3a / paho-mqtt3as / paho-mqtt3c / paho-mqtt3cs
libfind_pkg_detect(Paho paho FIND_PATH MQTTClient.h FIND_LIBRARY paho-mqtt3a)

if (NOT Paho_VERSION)
  # TODO: find proper version string
  # set(Paho_VERSION 1.0.3)
endif()

libfind_process(Paho)
