
set (MIDILISTENER_SRC
  src/midilistener.cpp
  src/MidiMqttRetransmitter.cpp
)
add_executable (midilistener ${MIDILISTENER_SRC})
include_directories (include)

# C++11
set_property(TARGET midilistener PROPERTY CXX_STANDARD 11)
set_property(TARGET midilistener PROPERTY CXX_STANDARD_REQUIRED 1)

# Dependencies
include_directories (
  ${RtMidi_INCLUDE_DIRS}
  ${PahoCpp_INCLUDE_DIRS}
  ${Json_INCLUDE_DIRS}
  ${B64_INCLUDE_DIRS}
)
target_link_libraries (midilistener
  ${RtMidi_LIBRARIES}
  ${PahoCpp_LIBRARIES}
  ${Json_LIBRARIES}
  ${B64_LIBRARIES}
)
