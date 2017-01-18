#set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}
#  CACHE INTERNAL "Path to generated protobuf files.")

file(GLOB proto-schema *.proto)
string(REGEX REPLACE "\\.[^.]*$" ".pb.h" proto-hpp ${proto-schema})
string(REGEX REPLACE "\\.[^.]*$" ".pb.cc" proto-cpp ${proto-schema})

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_CPP PROTO_HPP proto-schema)

add_custom_command(
  OUTPUT ${proto-hpp} ${proto-cpp}
  COMMAND protoc --cpp_out=${PROJECT_BINARY_DIR}/src/stan/proto
  --proto_path=${PROJECT_SOURCE_DIR}/proto ${proto-schema}
)

add_custom_target(
  proto-headers ALL DEPENDS ${proto-hpp} ${proto-cpp}
)



