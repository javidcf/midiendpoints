
cmake_minimum_required (VERSION 2.6)

project (MidiEndpoints)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Strict warnings
if (CMAKE_COMPILER_IS_GNUCC)
    set(warnings "-Wall -Wextra -Werror")
elseif (MSVC)
    set(warnings "/W4 /WX /EHsc")
endif()
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${warnings}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warnings}")

# Dependencies
# Versions are not being properly set right now...

#set (BSF_VERSION 0.0)
find_package (BSF ${BSF_VERSION} REQUIRED)

#set (RtMidi_VERSION 2.1.0)
find_package (RtMidi ${RtMidi_VERSION} REQUIRED)

#set (Mosquittopp_VERSION 1.4.3)
find_package (Mosquittopp ${Mosquittopp_VERSION} REQUIRED)

#set (Log4cxx_VERSION 0.10.0)
find_package (Log4cxx ${Log4cxx_VERSION} REQUIRED)

#set (Boost_VERSION 1.58)
find_package (Boost ${Boost_VERSION} REQUIRED COMPONENTS program_options)

#set (Asio_VERSION 1.10.6)
find_package (Asio ${Asio_VERSION} REQUIRED)

find_package (Protobuf REQUIRED)

set(Common_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/common/include;${CMAKE_BINARY_DIR}")

add_subdirectory (midilistener)
add_subdirectory (midiemitter)
