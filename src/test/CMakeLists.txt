enable_testing()
find_package(GTest REQUIRED)

# Finding stuff...
message("PROTOBUF_LIB_DIR: " ${PROTOBUF_LIB_DIR})
find_library(LIB_PROTOBUF NAMES protobuf libprotobuf protobuf.a libprotobuf.a PATHS ${PROTOBUF_LIB_DIR})
message("LIB_PROTOBUF: " ${LIB_PROTOBUF})
message("GTEST_LIB_DIR: " ${GTEST_LIB_DIR})
find_library(LIB_GTEST NAMES gtest libgtest gtest.a libgtest.a PATHS ${GTEST_LIB_DIR})
message("LIB_GTEST: " ${LIB_GTEST})
find_library(LIB_GTEST_MAIN NAMES gtest_main libgtest gtest.a libgtest.a PATHS ${GTEST_LIB_DIR})
message("LIB_GTEST_MAIN: " ${LIB_GTEST_MAIN})
message("STAN_LIB_DIR: " ${STAN_LIB_DIR})
find_library(LIB_STANC NAMES stanc libstanc stanc.a libstanc.a PATHS ${STAN_LIB_DIR})
message("LIB_STANC: " ${LIB_STANC})
message("GTEST_INCLUDE_DIR: " ${GTEST_INCLUDE_DIR})
file(GLOB TESTS_CPP *test.cpp)

# Build:
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR}/src)
include_directories(${GTEST_INCLUDE_DIR})
file(GLOB PROTO_CPP ${PROJECT_BINARY_DIR}/src/stan/proto/*.pb.cc)
file(GLOB PROTO_HPP ${PROJECT_BINARY_DIR}/src/stan/proto/*.pb.h)
foreach(TEST_CPP IN ITEMS ${TESTS_CPP})
  get_filename_component(TEST_NAME ${TEST_CPP} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_CPP} ${PROTO_CPP} ${STAN_SRC})
  set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/test/unit)
  add_dependencies(${TEST_NAME} proto-headers stanc)
  target_link_libraries(${TEST_NAME} ${LIB_GTEST} ${LIB_STANC} ${LIB_PROTOBUF})
  add_test(${TEST_NAME} ${PROJECT_BINARY_DIR}/test/unit/${TEST_NAME})
  GTEST_ADD_TESTS(${TEST_NAME} "" AUTO)
endforeach(TEST_CPP)




