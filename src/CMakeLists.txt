cmake_minimum_required(VERSION 2.8)
project(RocketSim)

find_package(GSL REQUIRED)
include_directories(${GSL_INLCUDE_DIRS})

#### main rocket executable
set(ROCKETSIM_SRC main.cpp)
add_executable(rocketsim ${ROCKETSIM_SRC})
target_link_libraries(rocketsim ${GSL_LIBRARIES})
set_property(TARGET rocketsim PROPERTY CXX_STANDARD 11)
