#set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}
#  CACHE INTERNAL "Path to generated protobuf files.")

file(GLOB proto-schema *.proto)

foreach(proto IN ITEMS ${proto-schema})
  get_filename_component(proto-stub ${proto} NAME)
  string(REGEX REPLACE ".proto$" ".pb.h" proto-hpp ${proto-stub})
  list(APPEND proto-hpp-list ${PROJECT_BINARY_DIR}/src/stan/proto/${proto-hpp})
  string(REGEX REPLACE ".proto$" ".pb.cc" proto-cpp ${proto-stub})
  list(APPEND proto-cpp-list ${PROJECT_BINARY_DIR}/src/stan/proto/${proto-cpp})
  message("PROTO_HPP: " ${proto-hpp})
  message("PROTO_CPP: " ${proto-cpp})
  add_custom_command(
    OUTPUT ${proto-hpp} ${proto-cpp}
    COMMAND protoc --cpp_out=${PROJECT_BINARY_DIR}/src/stan/proto
    --proto_path=${PROJECT_SOURCE_DIR}/proto ${proto}
    DEPENDS ${proto-schema}
  )
endforeach(proto)

#find_package(Protobuf REQUIRED)
#protobuf_generate_cpp(PROTO_CPP_SRC PROTO_HPP_SRC ${proto-schema})


add_custom_target(
  proto-headers ALL DEPENDS ${proto-hpp-list} ${proto-cpp-list} protobuf
)

set(PROTO_HPP ${proto-hpp-list} PARENT_SCOPE)
set(PROTO_CPP ${proto-cpp-list} PARENT_SCOPE)


