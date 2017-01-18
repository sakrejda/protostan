cmake_minimum_required (VERSION 3.5)
project (protostan VERSION 0.0.0.9000)

set(GTEST_BASE lib/stan/lib/stan_math/lib/gtest_1.7.0)
set(GTEST_INCLUDE ${GTEST_BASE}/include)

include_directories(${GTEST_INCLUDE} ${GTEST_BASE})
add_library(gtest STATIC ${GTEST_BASE}/src/gtest-all.cc)

#enable_testing()
#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE})

add_subdirectory(proto)
add_subdirectory(src)
#include_directories(${ProtobufIncludePath})
#find_package(Protobuf REQUIRED)

#include_directories(proto)
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS foo.proto)
#protobuf_generate_python(PROTO_PY foo.proto)

