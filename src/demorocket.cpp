/**
* Parts of this code are based on material provided in UOIT Graphics courses.
**/

#include "demorocket.hpp"

// STD
#include <iostream>
#include <string>
#include <vector>

// OpenGL
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/freeglut_ext.h>

// project
#include "meshdata.hpp"
#include "vao.hpp"

// global values
static const std::string FRAGMENT_SHADER_PATH = "../render.fs";
static const std::string VERTEX_SHADER_PATH = "../render.vs";
static std::vector<RSimView::VertexArrayObject> VAO_LIST;

// read in that shader file guy
std::string readShaderFile(const std::string& filename) {
    // try to open file
    FILE* fid = fopen(filename.c_str(), "r");
    if(fid == NULL) {
        std::cerr << "Error opening shader file: " << filename << std::endl;
        return "";
    } else {
        // skip to end
        fseek(fid, 0, SEEK_END);

        // here's the length
        int length = ftell(fid);
        rewind(fid);

        // write to buffer
        char* buffer = new char[length + 1];
        int written_count = fread(buffer, sizeof(char), length, fid);
        buffer[written_count] = 0;
        
        // copy to a string and return
        std::string ret(buffer, written_count-1);
        delete[] buffer;
        return ret;
    }
}

// build that shader, guy!
int buildShader(int type, const std::string& filename) {
    // read in your source
    std::string source = readShaderFile(filename);

    // create program
    int shader = glCreateShader(type);
    const char* source_ptr = source.c_str();
    glShaderSource(shader, 1, (const GLchar**)&source_ptr, 0);
    glCompileShader(shader);
    
    // check compilation
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(result != GL_TRUE) {
        // figure out how long the error message is
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &result);

        // print out the error message
        char* buffer = new char[result];
        glGetShaderInfoLog(shader, result, 0, buffer);
        std::cerr << "error compiling shader: " << filename << std::endl 
            << buffer << std::endl;

        // we're done here
        delete[] buffer;
        return 0;
    } else {
        return shader;
    }
}

int buildProgram(int vs, int fs) {
    // create and link program
    int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    // check shaders
    int vs_type, fs_type;
    glGetShaderiv(vs, GL_SHADER_TYPE, &vs_type);
    glGetShaderiv(fs, GL_SHADER_TYPE, &fs_type);
    if(vs_type != GL_VERTEX_SHADER) {
        printf("no vertex shader\n");
    }
    if(fs_type != GL_FRAGMENT_SHADER) {
        printf("no fragment shader\n");
    }
    

    // link program
    glLinkProgram(program);

    // check result
    int result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if(result != GL_TRUE) {
        // figure out how long the error message is
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &result);

        // print out the error message
        char* buffer = new char[result];
        glGetProgramInfoLog(program, result, 0, buffer);
        std::cerr << "error linking program: " << std::endl 
            << buffer << std::endl;

        // we're done here
        delete[] buffer;
        return 0;
    } else {
        return program;
    }
}

namespace window {
void onDisplay(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(RSimView::VertexArrayObject vao : VAO_LIST) {
        glUseProgram(vao.program);
        glBindVertexArray(vao.id);
        glDrawElements(GL_TRIANGLES, vao.index_count, GL_UNSIGNED_INT, NULL);
    }
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
    // load payload
    RSimView::MeshData payload_mesh = RSimView::payloadMeshData();
    
    // setup glut
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(640,640);
    glutCreateWindow("Rocket Science");

    // check for error
    GLenum error = glewInit();
    if(error != GLEW_OK) {
        std::cerr << "Tried to start GLEW, but then this happened: " 
            << glewGetErrorString(error) << std::endl;
        return 1;
    }

    // load shaders
    int fragment_shader = buildShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_PATH);
    int vertex_shader = buildShader(GL_VERTEX_SHADER, VERTEX_SHADER_PATH);
    if(fragment_shader == 0) {
        printf("error loading fragment shader\n");
    }
    if(vertex_shader == 0) {
        printf("error loading vertex shader\n");
    }


    // setup program
    GLuint program = buildProgram(vertex_shader, fragment_shader);
    RSimView::VertexArrayObject payload_vao = RSimView::loadMeshIntoBuffer(payload_mesh, program);
    VAO_LIST.push_back(payload_vao);
    
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
