#ifndef RSIM_MESHDATA_HPP
#define RSIM_MESHDATA_HPP
/** Check it out I hardcoded some meshes of a rocket and payload that I modelled - Kathryn **/

// C standard library
#include <cstddef>

// OpenGL
#include <GL/glew.h>
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
    MeshData(FloatT* pvertices, FloatT* pnormals, SizeT pvertex_length, IndexT* pindices, SizeT index_length);

    FloatT* vertices;
    FloatT* normals;
    IndexT* indices;
    SizeT vertex_length;
    SizeT index_count;
};

/// Build the vertices and such for a satellite payload thing
MeshData payloadMeshData();

/// Build the vertices and such for a rocket
MeshData rocketMeshData();

/// First stage of rocket mesh
MeshData firstStageMeshData();

/// Second stage of rocket mesh
MeshData secondStageMeshData();

/// Build the vertices and such for a cube
MeshData cubeMeshData();

} // namespace RSimView


#endif // RSIM_MESHDATA_HPP
