enable_testing()

# Finding stuff...
file(GLOB TESTS_CPP *test.cpp)

# Includes
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR}/src)
include_directories(${GTEST_INCLUDE_DIR})
include_directories(${STAN_INCLUDE_DIR})
include_directories(${PROTOSTAN_INCLUDE_DIR})

# Build:
add_custom_command(
  OUTPUT ${PROTO_CPP} ${PROTO_HPP}
  COMMAND protoc --cpp_out=${PROJECT_BINARY_DIR}/src/stan/proto
  --proto_path=${PROJECT_SOURCE_DIR}/proto ${proto-schema}
  DEPENDS ${proto-schema}
)

foreach(TEST_CPP IN ITEMS ${TESTS_CPP})
  get_filename_component(TEST_NAME ${TEST_CPP} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_CPP} ${STAN_SRC} ${PROTO_CPP})
  set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/test/unit)
  add_dependencies(${TEST_NAME} protostan-prerequisites proto-headers)
  target_link_libraries(${TEST_NAME} ${GTEST_LIBRARY} ${LIB_STANC} ${PROTOBUF_LIBS})
  add_test(${TEST_NAME} ${PROJECT_BINARY_DIR}/test/unit/${TEST_NAME})
  GTEST_ADD_TESTS(${TEST_CPP} "" AUTO)
endforeach(TEST_CPP)




