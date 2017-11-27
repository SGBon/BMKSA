#include "meshdata.hpp"

namespace RSimView {
static const SizeT PAYLOAD_VERTEX_COUNT = 9;
static const SizeT PAYLOAD_FACE_COUNT = 8;
static const SizeT PAYLOAD_VERTEX_LENGTH = PAYLOAD_VERTEX_COUNT*3;
static const SizeT PAYLOAD_FACE_LENGTH = PAYLOAD_FACE_COUNT*3;

static FloatT PAYLOAD_VERTICES[PAYLOAD_VERTEX_LENGTH] =
    // top
    {  0.00,  0.00, 0.00 

    // layer 1
    ,  1.00,  0.00,  1.00
    ,  0.71, -0.71,  1.00
    ,  0.00, -1.00,  1.00
    , -0.71, -0.71,  1.00
    , -1.00,  0.00,  1.00
    , -0.71,  0.71,  1.00
    ,  0.00,  1.00,  1.00
    ,  0.71,  0.71,  1.00
    
    // end of payload vertices
    };

static FloatT PAYLOAD_NORMALS[PAYLOAD_VERTEX_LENGTH];
static IndexT PAYLOAD_INDICES[PAYLOAD_FACE_LENGTH] = 
    // roof
    { 0, 1, 2
    , 0, 2, 3
    , 0, 3, 4
    , 0, 4, 5
    , 0, 5, 6
    , 0, 6, 7
    , 0, 7, 8
    , 0, 8, 1
    
    // end of payload indices
    };


MeshData::MeshData(FloatT* pvertices, FloatT* pnormals, SizeT pvertex_length, IndexT* pindices, SizeT pindex_count)
    : vertices(pvertices), normals(pnormals)
        , indices(pindices), vertex_length(pvertex_length), index_count(pindex_count)
    {}


MeshData payloadMeshData() {
    return MeshData(PAYLOAD_VERTICES, PAYLOAD_NORMALS, PAYLOAD_VERTEX_LENGTH, PAYLOAD_INDICES, PAYLOAD_FACE_LENGTH);
}

} // namespace RSimView