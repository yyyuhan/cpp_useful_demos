cmake_minimum_required(VERSION 3.5.1)

project(StreamingRPC CXX)

if(CMAKE_CROSSCOMPILING)
find_program(_PROTOBUF_PROTOC protoc)
else()
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()
set(_GRPC_GRPCPP grpc++)
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_REFLECTION grpc++_reflection)

# Proto file
get_filename_component(stream_proto "protos/streaming.proto" ABSOLUTE)
get_filename_component(stream_proto_path "${stream_proto}" PATH)

# Generated sources
set(stream_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/streaming.pb.cc")
set(stream_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/streaming.pb.h")
set(stream_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/streaming.grpc.pb.cc")
set(stream_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/streaming.grpc.pb.h")
add_custom_command(
      OUTPUT "${stream_proto_srcs}" "${stream_proto_hdrs}" "${stream_grpc_srcs}" "${stream_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${stream_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${stream_proto}"
      DEPENDS "${stream_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# stream_grpc_proto
add_library(stream_grpc_proto
  ${stream_grpc_srcs}
  ${stream_grpc_hdrs}
  ${stream_proto_srcs}
  ${stream_proto_hdrs})
#target_link_libraries(stream_grpc_proto
#  ${_REFLECTION}
#  ${_GRPC_GRPCPP}
#  ${_PROTOBUF_LIBPROTOBUF})

# Targets greeter_[async_](client|server)
foreach(_target
  stream_client server)
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(${_target}
    stream_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()
