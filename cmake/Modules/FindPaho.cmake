# - Try to find Paho
# Once done, this will define
#
#  Paho_FOUND - system has Paho
#  Paho_INCLUDE_DIRS - the Paho include directories
#  Paho_LIBRARIES -link these to use Paho

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Paho_PKGCONF Paho)

# Include dir
find_path(Paho_INCLUDE_DIR
  NAMES MQTTClient.h   # This should be enough right?
  PATHS ${Paho_PKGCONF_INCLUDE_DIRS}
)

# Finally the library itself
find_library(Paho_LIBRARY
  # which one of these??
  NAMES paho-mqtt3a  # asynchronous
  #NAMES paho-mqtt3as  # asynchronous + SLL
  #NAMES paho-mqtt3c  # classic/synchronous
  #NAMES paho-mqtt3cs  # classic/synchronous + SSL
  PATHS ${Paho_PKGCONF_LIBRARY_DIRS}
)

libfind_process(Paho)
