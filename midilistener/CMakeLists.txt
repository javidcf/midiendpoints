
set (MIDILISTENER_SRC midilistener.cpp)
add_executable (midilistener ${MIDILISTENER_SRC})

# Dependencies
find_package (RtMidi ${RTMIDI_VERSION} REQUIRED)
find_package (PahoCpp ${PAHOCPP_VERSION} REQUIRED)

include_directories (
  ${RtMidi_INCLUDE_DIRS}
  ${PahoCpp_INCLUDE_DIRS}
)
target_link_libraries (midilistener
  ${RtMidi_LIBRARIES}
  ${PahoCpp_LIBRARIES}
)
