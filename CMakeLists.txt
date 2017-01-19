cmake_minimum_required (VERSION 3.5)
project (protostan VERSION 0.0.0.9000)
cmake_policy(SET CMP0048 OLD)
include(CTest)

# Add Stan math
set(STAN_MATH lib/stan/lib/stan_math)
include_directories(${STAN_MATH})

# Include Boost
set(BOOST lib/stan/lib/stan_math/lib/boost_1.58.0)
include_directories(${BOOST})

# Find libraries:
find_library(LIB_PROTOBUF protobuf)
find_library(LIB_GTEST gtest ${PROJECT_BINARY_DIR}/lib/googletest/googletest)
find_library(LIB_GTEST_MAIN gtest_main ${PROJECT_BINARY_DIR}/lib/googletest/googletest)
find_library(LIB_STANC stanc ${PROJECT_BINARY_DIR}/lib)

message(${LIB_PROTOBUF})
message(${LIB_GTEST})
message(${LIB_GTEST_MAIN})
message(${LIB_STANC})

add_subdirectory(proto)
add_subdirectory(src)





