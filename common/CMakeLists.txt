
set (COMMON_HDRS
  include/MidiEndpointCommon.h
)

set (COMMON_SRCS
)

set (Common_NAME common)

protobuf_generate_cpp (PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/proto/masmusic.proto)

add_library (${Common_NAME} STATIC ${COMMON_HDRS} ${COMMON_SRCS} ${PROTO_SRCS} ${PROTO_HDRS})
include_directories (include ${CMAKE_CURRENT_BINARY_DIR})

# C++11
set_property(TARGET ${Common_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${Common_NAME} PROPERTY CXX_STANDARD_REQUIRED 1)

# Dependencies
include_directories (
  ${PROTOBUF_INCLUDE_DIRS}
)
target_link_libraries (${Common_NAME}
  ${PROTOBUF_LIBRARIES}
)

# Set library variables in parent scope
set (Common_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
PARENT_SCOPE)
set (Common_LIBRARIES
    ${Common_NAME}
PARENT_SCOPE)
