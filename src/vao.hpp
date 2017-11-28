#ifndef RSIM_VAO_HPP
#define RSIM_VAO_HPP

#include "meshdata.hpp"

namespace RSimView {
/**
* This is a mesh loaded into openGl's buffer.
**/
class VertexArrayObject {
public:
    GLuint program;
    GLsizei index_count;
    GLuint id; // vao index
};

VertexArrayObject loadMeshIntoBuffer(MeshData& data, GLuint program);

} // namespace RSimView



#endif //RSIM_VAO_HPP