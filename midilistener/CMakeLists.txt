
set (MIDILISTENER_HDRS
  include/MusicSensor.h
  include/detail/MusicSensor.h
)

set (MIDILISTENER_SRCS 
  src/midilistener.cpp
)

add_executable (midilistener ${MIDILISTENER_SRCS} ${MIDILISTENER_HDRS})
include_directories (include)

# C++11
set_property(TARGET midilistener PROPERTY CXX_STANDARD 11)
set_property(TARGET midilistener PROPERTY CXX_STANDARD_REQUIRED 1)

# Dependencies
include_directories (
  ${Common_INCLUDE_DIRS}
  ${BSF_INCLUDE_DIRS}
  ${RtMidi_INCLUDE_DIRS}
  ${Log4cxx_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${PROTOBUF_INCLUDE_DIRS}
  ${B64_INCLUDE_DIRS}
)
target_link_libraries (midilistener
  ${Common_LIBRARIES}
  ${BSF_LIBRARIES}
  ${RtMidi_LIBRARIES}
  ${Log4cxx_LIBRARIES}
  ${Boost_LIBRARIES}
  ${PROTOBUF_LIBRARIES}
  ${B64_LIBRARIES}
)
