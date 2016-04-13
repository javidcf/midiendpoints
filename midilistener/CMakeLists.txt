
set (MIDILISTENER_SRC
  src/midilistener.cpp
)

protobuf_generate_cpp (PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/proto/masmusic.proto)

add_executable (midilistener ${MIDILISTENER_SRC} ${PROTO_SRCS} ${PROTO_HDRS})
include_directories (include ${Common_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

# C++11
set_property(TARGET midilistener PROPERTY CXX_STANDARD 11)
set_property(TARGET midilistener PROPERTY CXX_STANDARD_REQUIRED 1)

# Dependencies
include_directories (
  ${BSF_INCLUDE_DIRS}
  ${RtMidi_INCLUDE_DIRS}
  ${Log4cxx_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${PROTOBUF_INCLUDE_DIRS}
  ${B64_INCLUDE_DIRS}
)
target_link_libraries (midilistener
  ${BSF_LIBRARIES}
  ${RtMidi_LIBRARIES}
  ${Log4cxx_LIBRARIES}
  ${Boost_LIBRARIES}
  ${PROTOBUF_LIBRARIES}
  ${B64_LIBRARIES}
)
