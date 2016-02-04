
cmake_minimum_required (VERSION 2.6)

project (MidiEndpoints)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set (RTMIDI_VERSION 2.1.0)
set (PAHOCPP_VERSION 0.1)
add_subdirectory (lib)

add_subdirectory (midilistener)
add_subdirectory (midiemitter)
