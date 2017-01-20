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
#find_package(Protobuf)
#find_package(GTest)
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


add_subdirectory(proto)
add_subdirectory(src)





