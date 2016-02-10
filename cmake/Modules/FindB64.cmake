# - Try to find B64
# Once done, this will define
#
#  B64_FOUND - system has B64
#  B64_INCLUDE_DIRS - the B64 include directories
#  B64_LIBRARIES -link these to use B64

include(LibFindMacros)

libfind_pkg_detect(B64 b64 FIND_PATH b64/encode.h b64/decode.h FIND_LIBRARY b64)

if (NOT B64_VERSION)
  # TODO: find proper version string
  #set(B64_VERSION 1.2)
endif()

libfind_process(B64)
