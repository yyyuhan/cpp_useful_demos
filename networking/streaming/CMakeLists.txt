cmake_minimum_required(VERSION 3.5.1)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set (CMAKE_CXX_STANDARD 17)
endif()

project(StreamingRPC CXX)

find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_program(_PROTOBUF_PROTOC protoc)
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
set(_GRPC_GRPCPP gRPC::grpc++)
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)


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
#include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# stream_grpc_proto
add_library(stream_grpc_proto
  ${stream_grpc_srcs}
  ${stream_grpc_hdrs}
  ${stream_proto_srcs}
  ${stream_proto_hdrs})
# target_link_libraries(stream_grpc_proto
#  ${_REFLECTION}
#  ${_GRPC_GRPCPP}
#  ${_PROTOBUF_LIBPROTOBUF})

# Include ProtoBuf headers
target_include_directories(stream_grpc_proto PRIVATE ${Protobuf_INCLUDE_DIRS})

# Targets stream_[async_](client|server)
foreach(_target
  stream_client)
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(${_target}
    stream_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()
