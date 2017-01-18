
file(GLOB TESTS_CPP *-test.cpp)
foreach (PATH IN ${TEST_CPP})
  get_filename_component(NAME ${PATH} NAME_WE)
  list(APPEND TESTS ${NAME})
endforeach(PATH)

foreach(test IN ${TESTS})
  message(${test})
  add_executable(${test} ${test}.cpp)
  add_dependencies(${test} gtest)
  target_link_libraries(${test} ${GTEST_LIB})
endforeach(test)




