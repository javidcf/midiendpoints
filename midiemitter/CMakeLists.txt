
set (MIDIEMITTER_HDRS
  include/MusicSensorClient.h
  include/detail/MusicSensorClient.h
)

set (MIDIEMITTER_SRCS
  src/midiemitter.cpp
)

add_executable (midiemitter ${MIDIEMITTER_SRCS} ${MIDIEMITTER_HDRS})
include_directories (include)

# C++11
set_property(TARGET midiemitter PROPERTY CXX_STANDARD 11)
set_property(TARGET midiemitter PROPERTY CXX_STANDARD_REQUIRED 1)

# Dependencies
include_directories (
  ${Common_INCLUDE_DIRS}
  ${BSF_INCLUDE_DIRS}
  ${RtMidi_INCLUDE_DIRS}
  ${Asio_INCLUDE_DIR}
  ${Log4cxx_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${PROTOBUF_INCLUDE_DIRS}
  ${B64_INCLUDE_DIRS}
)
target_link_libraries (midiemitter
  ${Common_LIBRARIES}
  ${BSF_LIBRARIES}
  ${RtMidi_LIBRARIES}
  ${Asio_LIBRARIES}
  ${Log4cxx_LIBRARIES}
  ${Boost_LIBRARIES}
  ${PROTOBUF_LIBRARIES}
  ${B64_LIBRARIES}
)
include (UseAsio)
