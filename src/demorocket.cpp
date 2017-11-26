#include "demorocket.hpp"

// STD
#include <iostream>

// OpenGL
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/freeglut_ext.h>


int demoRocket(Rocket& rocket, int* argc, char** argv) {
    // setup glut
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(640,640);
    glutCreateWindow("Rocket Science");

    // check for error
    GLenum error = glewInit();
    if(error != GLEW_OK) {
        std::cout << "Tried to start GLEW, but then this happened: " << glewGetErrorString(error) << std::endl;
        return 1;
    }
    
    // hookup glut functions
    
    // enable things
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.6,0.6,0.6,1.0);
    
    // startup main loop
    glutMainLoop();

    rocket.print();
    /* run for 100 seconds */
    for(int i = 0; i < 1000; ++i){
    rocket.step();
    }
    rocket.print();
    return 0;
}
