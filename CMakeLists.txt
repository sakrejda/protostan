cmake_minimum_required (VERSION 3.5)
project (protostan VERSION 0.0.0.9000)


add_subdirectory(proto)
add_subdirectory(src)
#include_directories(${ProtobufIncludePath})
#find_package(Protobuf REQUIRED)

#include_directories(proto)
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS foo.proto)
#protobuf_generate_python(PROTO_PY foo.proto)

