cmake_minimum_required(VERSION 2.8.4)
project(RocketSim)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GSL REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${GSL_INLCUDE_DIRS})

#### main rocket executable
set(ROCKETSIM_SRC main.cpp rigidbody.cpp rocket.cpp demorocket.cpp)
add_executable(rocketsim ${ROCKETSIM_SRC})
target_link_libraries(rocketsim ${OPENGL_gl_LIBRARY} ${GSL_LIBRARIES} ${GLUT_glut_LIBRARY} ${GLEW_LIBRARY})
set_property(TARGET rocketsim PROPERTY CXX_STANDARD 11)