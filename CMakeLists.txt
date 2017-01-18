cmake_minimum_required (VERSION 3.5)
project (protostan VERSION 0.0.0.9000)
cmake_policy(SET CMP0048 OLD)
include(CTest)

# Build GoogleTest lib.
set(GTEST_BASE lib/googletest)
set(GTEST_INCLUDE ${GTEST_BASE}/googletest/include)
include_directories(${GTEST_INCLUDE} ${GTEST_BASE})
#add_subdirectory(${PROJECT_SOURCE_DIR}/lib/googletest/googletest)
#add_library(gtest STATIC ${GTEST_BASE}/src/gtest-all.cc)
#set_target_properties(gtest PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
#  ${PROJECT_BINARY_DIR}/test/lib)
set(GTEST_ROOT ${GTEST_BASE})
set(GTEST_INCLUDE_DIR ${GTEST_INCLUDE})
set(GTEST_LIBRARY ${GTEST_BASE}/libgtest.a)
set(GTEST_MAIN_LIBRARY ${GTEST_BASE}/libgtest-main.s)

# Build Stanc lib.
set(STAN_BASE lib/stan)
set(STAN_SRC ${STAN_BASE}/src)
include_directories(${STAN_SRC})
file(GLOB STAN_GRAMMAR 
  ${STAN_SRC}/stan/lang/grammars/*_inst.cpp
  ${STAN_SRC}/stan/lang/grammars/*_def.cpp
  ${STAN_SRC}/stan/lang/*_inst.cpp
  ${STAN_SRC}/stan/lang/*_def.cpp
)
add_library(stanc STATIC ${STAN_GRAMMAR})
set_target_properties(stanc PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/lib)

# Add Stan math
set(STAN_MATH lib/stan/lib/stan_math)
include_directories(${STAN_MATH})

# Include Boost
set(BOOST lib/stan/lib/stan_math/lib/boost_1.58.0)
include_directories(${BOOST})

# Find libraries:
find_library(LIB_PROTOBUF protobuf)
find_library(LIB_GTEST gtest
  ${PROJECT_BINARY_DIR}/lib/googletest/googletest)
set(LIB_GTEST ${PROJECT_BINARY_DIR}/lib/googletest/libgtest.a)
#find_library(LIB_STANC stanc ${PROJECT_BINARY_DIR}/lib)
set(LIB_STANC ${PROJECT_BINARY_DIR}/lib/libstanc.a)

message(${LIB_PROTOBUF})
message(${LIB_GTEST})
message(${LIB_STANC})

add_subdirectory(proto)
add_subdirectory(src)





