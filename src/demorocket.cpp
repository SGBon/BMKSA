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
#include "glm/ext.hpp"

// project
#include "meshdata.hpp"
#include "vao.hpp"
#include "rocket.hpp"
#include "tiny_obj_loader.h"
#include "earth.hpp"
#include "common.hpp"

// global constants
static const std::string FRAGMENT_SHADER_PATH = "../render.fs";
static const std::string VERTEX_SHADER_PATH = "../render.vs";
static const char* UMODELVIEW = "uModelView";
static const char* UPROJECTION = "uProjection";
static const char* UCOLOR = "uColor";

/// colours are like ROY G BIV
static const glm::vec4 STAGE_COLOURS[Rocket::NUMBER_OF_STAGES] = {
  glm::vec4(1.0, 0.2, 0.2, 1.0)
  , glm::vec4(1.0, 0.5, 0.2, 1.0)
  , glm::vec4(1.0, 1.0, 0.2, 1.0)
  , glm::vec4(0.1, 0.8, 0.1, 1.0)
  , glm::vec4(0.2, 0.2, 1.0, 1.0)
};

// global variables
static std::vector<RSimView::VertexArrayObject> VAO_LIST;
static Rocket* ROCKET_MODEL = NULL;
static int ROCKET_ITER = 0;
static glm::mat4 ROTATION_MATRIX;
static bool USE_SPREADSHEET = false;

// updates rocket VAO to reflect changes
void updateView(double height, glm::vec3 thrust_direction, int stage) {
    double max_height = 100*1e3;
    glm::vec3 ground_color(205.0/255, 111.0/255, 1.0);
    glm::vec3 space_color(33.0/255, 27.0/255, 53.0/255);
    //ROTATION_MATRIX = ROCKET_MODEL->getRotationMatrix(); //glm::orientation(thrust_direction, glm::vec3(0.0,1.0f,0.0));
    ROTATION_MATRIX = glm::rotate(glm::mat4(1.0),(float)M_PI/2,glm::vec3(1.0,0.0,0.0));
    //std::cout << "updateView" << std::endl;

    float percent_up = fmin(max_height, height)/max_height;
    glm::vec3 clear_color = (1-percent_up)*ground_color + percent_up*space_color;
    //printf("height %f percent %f\n", height, percent_up);

    /* TODO: differentiate rocket from earth better */
    for(int i = 0; i < 3; ++i){
      RSimView::VertexArrayObject *vao = &VAO_LIST[i];
      glm::vec4 position = ROCKET_MODEL->getPositionGLM();
      vao->translation = position;
    }

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
    PROJECTION = glm::perspective(45.0f, ratio, 1.0f, 10000000.0f);
}

void onDisplay(void) {
    // create view
    glm::vec3 rpos = ROCKET_MODEL->getPositionGLM();
    const float rad = (1.0 - fmin(normalize(rpos.y,0.0,10000),0.95))*500;
    glm::vec3 eye(rad,rpos.y,rad);
    //std::cout << "eye: " << eye.x << ", " << eye.y << ", " << eye.z << std::endl;
    glm::vec3 center(ROCKET_MODEL->getPositionGLM());
    glm::vec3 up(0.0f,1.0f,0.0f);
    glm::mat4 view(glm::lookAt(eye, center, up));
    glm::mat4 projection(PROJECTION);
    glm::mat4 modelView = view;
    glm::vec4 color = STAGE_COLOURS[ROCKET_MODEL->getStageProgress()];
    glm::vec4 earth_color = glm::vec4(0.0,0.8,0.2,1.0);
    glm::mat4 earth_model = glm::mat4(1.0)*view;

    // draw stuff
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int obj = 0;
    for(RSimView::VertexArrayObject vao : VAO_LIST) {
        // here's the program we're using
        glUseProgram(vao.program);

        glm::mat4 translation = glm::translate(vao.translation);
        glm::mat4 scale = glm::scale(vao.scale);
        glm::mat4 mvts;
        if(obj < 3){
          glm::mat4 intrans = glm::inverse(translation);
          glm::mat4 origin_rotation = ROTATION_MATRIX*intrans;
          mvts = view*scale*translation*ROTATION_MATRIX;
        }else{
          mvts = earth_model*translation*scale;
        }

        // get uniforms
        int uModelView = glGetUniformLocation(vao.program, UMODELVIEW);
        int uProjection = glGetUniformLocation(vao.program, UPROJECTION);
        int uColor = glGetUniformLocation(vao.program, UCOLOR);

        // set uniforms
        glUniformMatrix4fv(uModelView, 1, false, glm::value_ptr(mvts));
        glUniformMatrix4fv(uProjection, 1, false, glm::value_ptr(projection));
        if(obj < 3){
          glUniform4fv(uColor, 1, glm::value_ptr(color));
        }else{
          glUniform4fv(uColor, 1, glm::value_ptr(earth_color));
        }

        // draw it
        glBindVertexArray(vao.id);
        glDrawElements(GL_TRIANGLES, vao.index_count, GL_UNSIGNED_INT, NULL);
        ++obj;
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
    static const int max_iter = 300000;
    static const double max_height = 480*1e4;
    if(ROCKET_ITER >= max_iter) {
        // don't do anything
        return;
    }
    ROCKET_MODEL->step();
    double height = ROCKET_MODEL->getPositionGLM().y;
    glm::vec4 thrust_direction_vec4(ROCKET_MODEL->getThrustDirectionGLM());
    glm::vec3 thrust_direction(thrust_direction_vec4.x,thrust_direction_vec4.y,thrust_direction_vec4.z);
    int stage = 0;
    //printf("onIdle: step %d height %f rotation %f, %f, %f, %f\n", ROCKET_ITER, height, thrust_direction_vec4.x, thrust_direction_vec4.y, thrust_direction_vec4.z, thrust_direction_vec4.w);
    ROCKET_MODEL->print(USE_SPREADSHEET);
    updateView(height, thrust_direction, stage);
    ROCKET_ITER++;

    // stop after 100 secondsd
    if(ROCKET_ITER == max_iter || height >= max_height) {
        printf("done\n");
        exit(0);
    }
}

} // namespace window

int demoRocket(Rocket& rocket, bool use_spreadsheet, int* argc, char** argv) {
    // set that
    USE_SPREADSHEET = use_spreadsheet;

    // load payload
    RSimView::MeshData first_stage_mesh = RSimView::firstStageMeshData();
    RSimView::MeshData second_stage_mesh = RSimView::secondStageMeshData();
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
        return 1;
    }
    if(vertex_shader == 0) {
        printf("error loading vertex shader\n");
        return 1;
    }

    // setup program
    GLuint program = buildProgram(vertex_shader, fragment_shader);
    if(program == 0) {
        std::cerr << "error occured loading program" << std::endl;
        return 1;
    }

    RSimView::VertexArrayObject payload_vao = RSimView::loadMeshIntoBuffer(
        payload_mesh, program);
    RSimView::VertexArrayObject first_stage_vao =RSimView::loadMeshIntoBuffer(
        first_stage_mesh, program);
    RSimView::VertexArrayObject second_stage_vao =RSimView::loadMeshIntoBuffer(
        second_stage_mesh, program);
    VAO_LIST.push_back(payload_vao);
    VAO_LIST.push_back(first_stage_vao);
    VAO_LIST.push_back(second_stage_vao);

    /* load earth */
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err = tinyobj::LoadObj(shapes, materials, "sphere.obj", 0);
    if(err.size() != 0){
        std::cerr << "Error loading object file: " << err << std::endl;
        exit(-1);
    }

    RSimView::MeshData earth_mesh(shapes[0].mesh.positions.data(),shapes[0].mesh.normals.data(),
      shapes[0].mesh.positions.size(),shapes[0].mesh.indices.data(),shapes[0].mesh.indices.size());
    RSimView::VertexArrayObject earth_vao = RSimView::loadMeshIntoBuffer(earth_mesh,program);

    /* set location and scale of earth */

    earth_vao.translation = glm::vec3(0,earth.position[1],0);
    earth_vao.scale = glm::vec3(earth.radius,earth.radius,earth.radius);

    VAO_LIST.push_back(earth_vao);

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
