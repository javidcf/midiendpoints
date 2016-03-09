
set (MIDIEMITTER_SRC
  src/midiemitter.cpp
  src/MidiMqttPlayer.cpp
)
add_executable (midiemitter ${MIDIEMITTER_SRC})
include_directories (include ${Common_INCLUDE_DIRS})

# C++11
set_property(TARGET midiemitter PROPERTY CXX_STANDARD 11)
set_property(TARGET midiemitter PROPERTY CXX_STANDARD_REQUIRED 1)

# Dependencies
include_directories (
  ${RtMidi_INCLUDE_DIRS}
  ${Mosquittopp_INCLUDE_DIRS}
  ${Json_INCLUDE_DIRS}
  ${B64_INCLUDE_DIRS}
  ${Log4cxx_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)
target_link_libraries (midiemitter
  ${RtMidi_LIBRARIES}
  ${Mosquittopp_LIBRARIES}
  ${Json_LIBRARIES}
  ${B64_LIBRARIES}
  ${Log4cxx_LIBRARIES}
  ${Boost_LIBRARIES}
)
