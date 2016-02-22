# - Try to find Log4cxx
# Once done, this will define
#
#  Log4cxx_FOUND - system has Log4cxx
#  Log4cxx_INCLUDE_DIRS - the Log4cxx include directories
#  Log4cxx_LIBRARIES -link these to use Log4cxx

include(LibFindMacros)

libfind_pkg_detect(Log4cxx reckless FIND_PATH log4cxx/log4cxx.h FIND_LIBRARY log4cxx)

if (NOT Log4cxx_VERSION)
  # TODO: find proper version string
  # set(Log4cxx_VERSION 0.10.0)
endif()

libfind_process(Log4cxx)
