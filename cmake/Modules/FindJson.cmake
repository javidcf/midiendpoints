# - Try to find Json
# Once done, this will define
#
#  Json_FOUND - system has Json
#  Json_INCLUDE_DIRS - the Json include directories
#  Json_LIBRARIES -link these to use Json

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Json_PKGCONF Json)

# Include dir
find_path(Json_INCLUDE_DIR
  NAMES json.hpp
  # Should prioritize repo version?
  PATHS ${Json_PKGCONF_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/lib/json/include
)

# No libraries needed
set(Json_LIBRARY "")

if (Json_PKGCONF_VERSION)
  set(Json_VERSION PahoCpp_PKGCONF_VERSION)
else()
  # TODO: find proper version string
  set(Json_VERSION 2.0.0)
endif()

libfind_process(Json)
