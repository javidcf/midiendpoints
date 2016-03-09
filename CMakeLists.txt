
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
#set (RtMidi_VERSION 2.1.0)
#set (Mosquittopp_VERSION 1.4.3)
#set (JSON_VERSION 2.0.0)
#set (B64_VERSION 1.2)
#set (Log4cxx_VERSION 0.10.0)
#set (Boost_VERSION 1.58)
find_package (RtMidi ${RtMidi_VERSION} REQUIRED)
find_package (Mosquittopp ${Mosquittopp_VERSION} REQUIRED)
find_package (Json ${JSON_VERSION} REQUIRED)
find_package (B64 ${B64_VERSION} REQUIRED)
find_package (Log4cxx ${Log4cxx_VERSION} REQUIRED)
find_package (Boost ${Boost_VERSION} REQUIRED COMPONENTS program_options)

# add_subdirectory (lib)

set(Common_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/common/include")

add_subdirectory (midilistener)
add_subdirectory (midiemitter)
