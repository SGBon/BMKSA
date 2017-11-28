#include "vao.hpp"
#include <iostream>

namespace RSimView {
const char* VPOSITION = "vPosition";


VertexArrayObject loadMeshIntoBuffer(MeshData& data, GLuint program) {
    // generate vertex array
    VertexArrayObject vao;
    vao.program = program;
    vao.index_count = data.index_count;
    glGenVertexArrays(1, &vao.id);
    glBindVertexArray(vao.id);

    // figure out sizes
    std::cout << "figure out size" << std::endl;
    size_t position_buffer_size = data.vertex_length*sizeof(FloatT);
    size_t index_buffer_size = data.index_count*sizeof(IndexT);
    
    // generate vertex buffer
    std::cout << "generate vertex buffer" << std::endl;
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, position_buffer_size, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, position_buffer_size, data.vertices);

    // link up to program
    std::cout << "use program" << std::endl;
    glUseProgram(program);

    // vertex position
    std::cout << "position" << std::endl;
    GLint vPosition = glGetAttribLocation(program, VPOSITION);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);

    //  indices
    std::cout << "indices" << std::endl;
    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size
        , data.indices, GL_STATIC_DRAW);
    
    // done

    std::cout << "done with vao" << std::endl;
    return vao;
}
} // namespace RSimView