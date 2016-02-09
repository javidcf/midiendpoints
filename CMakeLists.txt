
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

set (RTMIDI_VERSION 2.1.0)
set (PAHOCPP_VERSION 0.1)
set (JSON_VERSION 2.0.0)  # Not really doing anything
add_subdirectory (lib)

add_subdirectory (midilistener)
add_subdirectory (midiemitter)
