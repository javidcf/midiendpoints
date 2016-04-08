# - Try to find Asio
# Once done, this will define
#
#  Asio_FOUND - system has Asio
#  Asio_INCLUDE_DIRS - the Asio include directories
#  Asio_LIBRARIES -link these to use Asio


include(LibFindMacros)

# Dependencies
libfind_package(Asio Threads)
set(Threads_LIBRARY "${CMAKE_THREAD_LIBS_INIT}")

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Asio_PKGCONF Asio)

# Include dir
find_path(Asio_INCLUDE_DIR
  NAMES asio.hpp
)

# No libraries needed
set(Asio_LIBRARY "")

if (NOT Asio_VERSION)
  # TODO: find proper version string
  #set(Asio_VERSION 1.10.6)
  # libfind_version_header(Asio asio/version.hpp ASIO_VERSION)
  # math(EXPR Asio_VERSION_MAJOR "${Asio_VERSION} / 100000")
  # math(EXPR Asio_VERSION_MINOR "(${Asio_VERSION} / 100) % 1000")
  # math(EXPR Asio_VERSION_SUBMINOR "${Asio_VERSION} % 100")
  # string(CONCAT Asio_VERSION ${Asio_VERSION_MAJOR} "."
  #                            ${Asio_VERSION_MINOR} "."
  #                            ${Asio_VERSION_SUBMINOR})
endif()

libfind_process(Asio)
