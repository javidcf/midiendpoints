
set (MIDIEMITTER_SRC
  src/midiemitter.cpp
)

protobuf_generate_cpp (PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/proto/masmusic.proto)

add_executable (midiemitter ${MIDIEMITTER_SRC} ${PROTO_SRCS} ${PROTO_HDRS})
include_directories (include ${Common_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

# C++11
set_property(TARGET midiemitter PROPERTY CXX_STANDARD 11)
set_property(TARGET midiemitter PROPERTY CXX_STANDARD_REQUIRED 1)

# Dependencies
include_directories (
  ${BSF_INCLUDE_DIRS}
  ${RtMidi_INCLUDE_DIRS}
  ${Asio_INCLUDE_DIR}
  ${Log4cxx_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${PROTOBUF_INCLUDE_DIRS}
)
target_link_libraries (midiemitter
  ${BSF_LIBRARIES}
  ${RtMidi_LIBRARIES}
  ${Asio_LIBRARIES}
  ${Log4cxx_LIBRARIES}
  ${Boost_LIBRARIES}
  ${PROTOBUF_LIBRARIES}
)
include (UseAsio)
