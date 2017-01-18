file(GLOB TESTS_CPP *test.cpp)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR}/src)
file(GLOB PROTO_CPP ${PROJECT_BINARY_DIR}/src/stan/proto/*.pb.cc)
file(GLOB PROTO_HPP ${PROJECT_BINARY_DIR}/src/stan/proto/*.pb.h)
foreach(TEST_CPP IN ITEMS ${TESTS_CPP})
  get_filename_component(TEST_NAME ${TEST_CPP} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_CPP} ${PROTO_CPP})
  set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/test/unit)
  add_dependencies(${TEST_NAME} gtest proto-headers stanc)
  target_link_libraries(${TEST_NAME} ${LIB_GTEST} ${LIB_STANC} ${LIB_PROTOBUF})
endforeach(TEST_CPP)




