#ifndef RSIM_MESHDATA_HPP
#define RSIM_MESHDATA_HPP
/** Check it out I hardcoded some meshes of a rocket and payload that I modelled - Kathryn **/

// C standard library
#include <cstddef>

// OpenGL
#include <GL/gl.h>


/// Namespace for rocket sim meshes
namespace RSimView {
// typedefs for GL appropriate numerical types
typedef GLfloat FloatT;
typedef GLuint IndexT;
typedef GLsizei SizeT;

/// Holds onto the points and orderings of a mesh to load into openGL.
class MeshData {
public:
    /// Obtain references to the different datatypes. 
    MeshData(FloatT* pvertices, FloatT* pnormals, SizeT pvertex_count, IndexT* pindices, SizeT pindex_count);

    FloatT* vertices;
    FloatT* normals;
    IndexT* indices;
    SizeT vertex_count;
    SizeT index_count;
};

/// Build the vertices and such for a satellite payload thing
MeshData payloadMeshData();

/// Build the vertices and such for a rocket
MeshData rocketMeshData();

} // namespace RSimView


#endif // RSIM_MESHDATA_HPP
