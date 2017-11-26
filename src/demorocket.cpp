#include "demorocket.hpp"

// STD
#include <iostream>

// OpenGL
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>


void demoRocket(Rocket& rocket, int* argc, char** argv) {
    // setup glut
    std::cout << "do stuff" << std::endl;
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(640,640);
    glutCreateWindow("Rocket Science");
    std::cout << "did stuff" << std::endl;

    // check for error
    //GLenum error = glewInit();
    //if(error != GLEW_OK)

    rocket.print();
    /* run for 100 seconds */
    for(int i = 0; i < 1000; ++i){
    rocket.step();
    }
    rocket.print();
}
