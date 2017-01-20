enable_testing()
find_package(GTest REQUIRED)

file(GLOB TESTS_CPP *test.cpp)
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
  add_dependencies(${TEST_NAME} proto-headers stan stanc)
  target_link_libraries(${TEST_NAME} ${LIB_GTEST} ${LIB_STANC} ${LIB_PROTOBUF})
  add_test(${TEST_NAME} ${PROJECT_BINARY_DIR}/test/unit/${TEST_NAME})
  GTEST_ADD_TESTS(${TEST_NAME} "" AUTO)
endforeach(TEST_CPP)




