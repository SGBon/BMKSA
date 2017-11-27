#include "demorocket.hpp"

// STD
#include <iostream>

// OpenGL
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/freeglut_ext.h>




namespace window {

void onDisplay(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSolidTeapot(10.0);
    glutSwapBuffers();
}

void onReshape(int width, int height) {
    glViewport(0,0,width,height);
}

void onVisibilityChange(int state) {
    //...
}
void onCharacterKeyEvent(unsigned char key, int mouseX, int mouseY) {
    //...
}

void onOtherKeyEvent(int key, int mouseX, int mouseY) {
    //,,,
}

void onIdle() {
    //...
}

} // namespace window




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
    glutDisplayFunc(window::onDisplay);
    glutReshapeFunc(window::onReshape);
    glutKeyboardFunc(window::onCharacterKeyEvent);
    glutSpecialFunc(window::onOtherKeyEvent);
    glutIdleFunc(window::onIdle);
    glutVisibilityFunc(window::onVisibilityChange);

    // enable things
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.6,0.6,0.6,1.0);
    
    // startup main loop
    glutMainLoop();
    return 0;
}
