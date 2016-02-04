
set (MIDIEMITTER_SRC midiemitter.cpp)
add_executable (midiemitter ${MIDIEMITTER_SRC})

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
