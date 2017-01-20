#set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}
#  CACHE INTERNAL "Path to generated protobuf files.")

file(GLOB proto-schema *.proto)

foreach(proto IN ITEMS ${proto-schema})
  string(REGEX REPLACE ".proto$" ".pb.h" proto-hpp ${proto})
  list(APPEND proto-hpp-list ${proto-hpp})
  string(REGEX REPLACE ".proto$" ".pb.cc" proto-cpp ${proto})
  list(APPEND proto-cpp-list ${proto-cpp})
endforeach(proto)

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_CPP PROTO_HPP proto-schema)

add_custom_command(
  OUTPUT ${proto-hpp-list} ${proto-cpp-list}
  COMMAND protoc --cpp_out=${PROJECT_BINARY_DIR}/src/stan/proto
  --proto_path=${PROJECT_SOURCE_DIR}/proto ${proto-schema}
  DEPENDS ${proto-schema}
)

add_custom_target(
  proto-headers ALL DEPENDS ${proto-hpp-list} ${proto-cpp-list} protobuf
)

set(PROTO_HPP ${proto-hpp-list} PARENT_SCOPE)
set(PROTO_CPP ${proto-cpp-list} PARENT_SCOPE)


