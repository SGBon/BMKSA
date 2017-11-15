cmake_minimum_required(VERSION 2.8)
project(RocketSim)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

#### main rocket executable
set(ROCKETSIM_SRC main.cpp)
add_executable(rocketsim ${ROCKETSIM_SRC})
set_property(TARGET rocketsim PROPERTY CXX_STANDARD 11)
