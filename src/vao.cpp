#include "vao.hpp"
#include <iostream>

namespace RSimView {
const char* VPOSITION = "vPosition";
const char* VNORMAL = "vNormal";

VertexArrayObject::VertexArrayObject(){
  this->translation = glm::vec3(0,0,0);
  this->scale = glm::vec3(1,1,1);
}


VertexArrayObject loadMeshIntoBuffer(MeshData& data, GLuint program) {
    // generate vertex array
    VertexArrayObject vao;
    vao.program = program;
    vao.index_count = data.index_count;
    glGenVertexArrays(1, &vao.id);
    glBindVertexArray(vao.id);

    // figure out sizes
    size_t position_buffer_size = data.vertex_length*sizeof(FloatT);
    size_t normal_buffer_size = position_buffer_size;
    size_t index_buffer_size = data.index_count*sizeof(IndexT);

    // generate vertex buffer
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, position_buffer_size+normal_buffer_size, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, position_buffer_size, data.vertices);
    glBufferSubData(GL_ARRAY_BUFFER, position_buffer_size, normal_buffer_size, data.normals);

    // link up to program
    glUseProgram(program);

    // vertex position
    GLint vPosition = glGetAttribLocation(program, VPOSITION);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);

    // vertex normal
    GLint vNormal= glGetAttribLocation(program, VNORMAL);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)position_buffer_size);
    glEnableVertexAttribArray(vNormal);

    //  indices
    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size
        , data.indices, GL_STATIC_DRAW);

    // done
    return vao;
}
} // namespace RSimView
