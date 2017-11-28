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

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

// project
#include "meshdata.hpp"
#include "vao.hpp"

// global values
static const std::string FRAGMENT_SHADER_PATH = "../render.fs";
static const std::string VERTEX_SHADER_PATH = "../render.vs";
static const char* UMODELVIEW = "uModelView";
static const char* UPROJECTION = "uProjection";
static std::vector<RSimView::VertexArrayObject> VAO_LIST;
static Rocket* ROCKET_MODEL = NULL;
static int ROCKET_ITER = 0; 
static glm::mat4 ROTATION_MATRIX;

// updates rocket VAO to reflect changes
void updateView(double height, glm::vec3 thrust_direction, int stage) {
    double max_height = 112*1e3;
    glm::vec3 ground_color(205.0/255, 111.0/255, 1.0);
    glm::vec3 space_color(33.0/255, 27.0/255, 53.0/255);
    ROTATION_MATRIX = glm::orientation(thrust_direction, glm::vec3(0,0,-1));
    //std::cout << "updateView" << std::endl;

    float percent_up = fmin(max_height, height)/max_height;
    glm::vec3 clear_color = (1-percent_up)*ground_color + percent_up*space_color;
    //printf("height %f percent %f\n", height, percent_up);


    glClearColor(clear_color.x,clear_color.y,clear_color.z,1.0);
    glutPostRedisplay();
}

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
glm::mat4 PROJECTION;
float CAMERA_LONGITUDE, CAMERA_COLATITUDE, CAMERA_RADIUS;

void updateProjection(int width, int height) {
    float ratio = 1.0f * width / height;
    PROJECTION = glm::perspective(45.0f, ratio, 1.0f, 200.0f);
}

void onDisplay(void) {
    // create view
    glm::vec3 eye(CAMERA_RADIUS*glm::vec3(
        cos(CAMERA_LONGITUDE)*sin(CAMERA_COLATITUDE)
        , sin(CAMERA_LONGITUDE)*sin(CAMERA_COLATITUDE)
        , cos(CAMERA_COLATITUDE)));
    //std::cout << "eye: " << eye.x << ", " << eye.y << ", " << eye.z << std::endl;
    glm::vec3 center(0.0f,0,20.0f);
    glm::vec3 up(0.0f,0.0,-1.0);
    glm::mat4 view(glm::lookAt(eye, center, up));
    glm::mat4 projection(PROJECTION);
    glm::mat4 modelView = ROTATION_MATRIX*view;

    // draw stuff
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(RSimView::VertexArrayObject vao : VAO_LIST) {
        // here's the program we're using
        glUseProgram(vao.program);

        // get uniforms
        int uModelView = glGetUniformLocation(vao.program, UMODELVIEW);
        int uProjection = glGetUniformLocation(vao.program, UPROJECTION);
        
        // set uniforms
        glUniformMatrix4fv(uModelView, 1, 0, glm::value_ptr(modelView));
        glUniformMatrix4fv(uProjection, 1, 0, glm::value_ptr(projection));

        // draw it
        glBindVertexArray(vao.id);
        glDrawElements(GL_TRIANGLES, vao.index_count, GL_UNSIGNED_INT, NULL);
    }
    glutSwapBuffers();
}

void onReshape(int width, int height) {
    glViewport(0,0,width,height);
    updateProjection(width, height);
}

void onVisibilityChange(int state) {
    //...
}
void onCharacterKeyEvent(unsigned char key, int mouseX, int mouseY) {
    //...
}

void onOtherKeyEvent(int key, int mouseX, int mouseY) {
    double dCol = 0.0;
    double dLong = 0.0;
    double step = 0.1;
    double pi = asin(1.0)*2;

    switch(key) {
        case GLUT_KEY_UP:
            dCol += step;
            break;

        case GLUT_KEY_DOWN:
            dCol -= step;
            break;

        case GLUT_KEY_RIGHT:
            dLong += step;
            break;

        case GLUT_KEY_LEFT:
            dLong -= step;
            break;

        default:
            break;
    }
    
    CAMERA_COLATITUDE = fmin(0.999*pi, fmax(0.001, CAMERA_COLATITUDE+dCol));
    CAMERA_LONGITUDE = fmin(1.999*pi, fmax(0, CAMERA_LONGITUDE+dLong));
    glutPostRedisplay();
    
}

void onIdle() {
    static const int max_iter = 100;
    if(ROCKET_ITER >= max_iter) {
        // don't do anything
        return;
    }
    ROCKET_MODEL->step();
    // TODO: get values from rocket
    double height = ROCKET_MODEL->getPositionGLM().y;
    glm::vec4 thrust_direction_vec4(ROCKET_MODEL->getThrustDirectionGLM());
    glm::vec3 thrust_direction(thrust_direction_vec4.x,1.0,-thrust_direction_vec4.y);
    int stage = 0;
    printf("onIdle: step %d height %f rotation %f, %f, %f, %f\n", ROCKET_ITER, height, thrust_direction_vec4.x, thrust_direction_vec4.y, thrust_direction_vec4.z, thrust_direction_vec4.w);
    updateView(height, thrust_direction, stage);
    ROCKET_ITER++;

    // stop after 100 seconds
    if(ROCKET_ITER == max_iter) {
        printf("done\n");
        //exit(0);
    }
}

} // namespace window

int demoRocket(Rocket& rocket, int* argc, char** argv) {
    // load payload
    RSimView::MeshData rocket_mesh = RSimView::rocketMeshData();
    RSimView::MeshData payload_mesh = RSimView::payloadMeshData();

    // set pointer
    ROCKET_MODEL = &rocket;

    // figure out window size
    int width = 640;
    int height = 640;

    // setup glut
    std::cout << "Initializing window" << std::endl;
    glutInit(argc, argv);
    glutInitContextVersion(3,3);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(width,height);
    window::updateProjection(width, height);
    window::CAMERA_COLATITUDE = asin(1.0);
    window::CAMERA_LONGITUDE = 0;
    window::CAMERA_RADIUS = 50;
    glutCreateWindow("Rocket Science");



    // check for error
    glewExperimental = true;
    GLenum error = glewInit();
    if(error != GLEW_OK) {
        std::cerr << "Tried to start GLEW, but then this happened: "
            << glewGetErrorString(error) << std::endl;
        return 1;
    }

    // load shaders
    std::cout << "Loading Shaders" << std::endl;
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
    if(program == 0) {
        std::cerr << "error occured loading program" << std::endl;
    }

    RSimView::VertexArrayObject payload_vao = RSimView::loadMeshIntoBuffer(
        payload_mesh, program);
    RSimView::VertexArrayObject rocket_vao =RSimView::loadMeshIntoBuffer(
        rocket_mesh, program);
    VAO_LIST.push_back(payload_vao);
    VAO_LIST.push_back(rocket_vao);

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
    std::cout << "Launching Simulation" << std::endl;
    glutMainLoop();
    return 0;
}
