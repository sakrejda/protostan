cmake_minimum_required (VERSION 3.5)
project (protostan VERSION 0.0.0.9000)
cmake_policy(SET CMP0048 OLD)
include(CTest)

# Include Stan math
include_directories(${STAN_MATH_INCLUDE_DIR})

# Include Stan
include_directories(${STAN_INCLUDE_DIR})

# Include Boost
set(BOOST $STAN/lib/stan/lib/stan_math/lib/boost_1.66.0)
include_directories(${BOOST})


add_subdirectory(proto)
add_subdirectory(src)





