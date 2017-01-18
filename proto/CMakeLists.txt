set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "Path to generated protobuf files.")

file(GLOB proto-schema *.proto)

add_custom_command(
  OUTPUT ${proto-schema}.pb.h ${proto-schema}.pb.cpp
  COMMAND protoc --cpp_out=${PROJECT_BINARY_DIR}/src/stan/proto
  --proto_path=${PROJECT_SOURCE_DIR}/proto ${proto-schema}
)

add_custom_target(
  proto-headers ALL DEPENDS ${proto-schema}.pb.h ${proto-schema}.pb.cpp
)



