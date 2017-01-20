enable_testing()

# Finding stuff...
file(GLOB TESTS_CPP *test.cpp)

# Includes
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR}/src)
include_directories(${GTEST_INCLUDE_DIR})
include_directories(${STAN_INCLUDE_DIR})

# Build:
message("STAN_INCLUDE_DIR: " ${STAN_INCLUDE_DIR})
message("PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR})
#file(GLOB PROTO_CPP ${PROJECT_BINARY_DIR}/src/stan/proto/*.pb.cc)
foreach(PROTO IN ITEMS ${PROTO_CPP})
  message("PROTO_CPP: " ${PROTO})
endforeach(PROTO)
foreach(PROTO IN ITEMS ${PROTO_HPP})
  message("PROTO_HPP: " ${PROTO})
endforeach(PROTO)

foreach(TEST_CPP IN ITEMS ${TESTS_CPP})
  get_filename_component(TEST_NAME ${TEST_CPP} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_CPP} ${PROTO_HPP} ${PROTO_CPP} ${STAN_SRC})
  set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/test/unit)
  add_dependencies(${TEST_NAME} protostan-prerequisites proto-headers)
  target_link_libraries(${TEST_NAME} ${GTEST_LIBRARY} ${LIB_STANC} ${PROTOBUF_LIBS})
  add_test(${TEST_NAME} ${PROJECT_BINARY_DIR}/test/unit/${TEST_NAME})
endforeach(TEST_CPP)

GTEST_ADD_TESTS(${TESTS_CPP} "" AUTO)



