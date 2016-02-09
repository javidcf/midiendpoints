
set (MIDIEMITTER_SRC
  src/midiemitter.cpp
)
add_executable (midiemitter ${MIDIEMITTER_SRC})

# C++11
set_property(TARGET midiemitter PROPERTY CXX_STANDARD 11)
set_property(TARGET midiemitter PROPERTY CXX_STANDARD_REQUIRED 1)

# Dependencies
find_package (RtMidi ${RTMIDI_VERSION} REQUIRED)
find_package (PahoCpp ${PAHOCPP_VERSION} REQUIRED)

include_directories (
  ${RtMidi_INCLUDE_DIRS}
  ${PahoCpp_INCLUDE_DIRS}
)
target_link_libraries (midiemitter
  ${RtMidi_LIBRARIES}
  ${PahoCpp_LIBRARIES}
)
