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
  NAMES mqttv3a  # asynchronous
  #NAMES mqttv3as  # asynchronous + SLL
  #NAMES mqttv3c  # classic/synchronous
  #NAMES mqttv3cs  # classic/synchronous + SSL
  PATHS ${Paho_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(Paho_PROCESS_INCLUDES Paho_INCLUDE_DIR)
set(Paho_PROCESS_LIBS Paho_LIBRARY)
libfind_process(Paho)
