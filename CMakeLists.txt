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
find_package(Protobuf)
find_package(GTest)
message("PROTOBUF_LIB_DIR: " ${PROTOBUF_LIB_DIR})
message("GTEST_LIB_DIR: " ${GTEST_LIB_DIR})
message("STAN_LIB_DIR: " ${STAN_LIB_DIR})
find_library(LIB_PROTOBUF protobuf PATHS ${PROTOBUF_LIB_DIR})
#find_library(LIB_GTEST gtest PATHS ${GTEST_LIB_DIR})
#find_library(LIB_GTEST_MAIN gtest_main PATHS ${GTEST_LIB_DIR})
find_library(LIB_STANC stanc PATHS ${STAN_LIB_DIR})


add_subdirectory(proto)
add_subdirectory(src)





