
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

# Versions are not being properly set right now...
#set (RTMIDI_VERSION 2.1.0)
#set (PAHOCPP_VERSION 0.1)
#set (JSON_VERSION 2.0.0)
#set (B64_VERSION 1.2)
find_package (RtMidi ${RTMIDI_VERSION} REQUIRED)
find_package (PahoCpp ${PAHOCPP_VERSION} REQUIRED)
find_package (Json ${JSON_VERSION} REQUIRED)
find_package (B64 ${B64_VERSION} REQUIRED)

# add_subdirectory (lib)

add_subdirectory (midilistener)
add_subdirectory (midiemitter)
