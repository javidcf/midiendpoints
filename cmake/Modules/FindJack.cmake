# - Try to find Jack
# Once done, this will define
#
#  Jack_FOUND - system has Jack
#  Jack_INCLUDE_DIRS - the Jack include directories
#  Jack_LIBRARIES -link these to use Jack

include(LibFindMacros)

libfind_pkg_detect(Jack jack FIND_PATH jack/jack.h FIND_LIBRARY jack)

# TODO: fix non found version?

libfind_process(Jack)
