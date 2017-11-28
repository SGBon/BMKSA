#include "meshdata.hpp"

namespace RSimView {
// Cube stuff
static const SizeT CUBE_VERTEX_COUNT = 8;
static const SizeT CUBE_FACE_COUNT = 6*2;
static const SizeT CUBE_VERTEX_LENGTH = CUBE_VERTEX_COUNT*3;
static const SizeT CUBE_FACE_LENGTH = CUBE_FACE_COUNT*3;

static FloatT CUBE_VERTICES[CUBE_VERTEX_LENGTH] =
    {  1,  1,  1
    ,  1, -1,  1
    , -1, -1,  1
    , -1,  1,  1
    ,  1,  1, -1
    ,  1, -1, -1
    , -1, -1, -1
    , -1,  1, -1
    };


static FloatT CUBE_NORMALS[CUBE_VERTEX_LENGTH];

static IndexT CUBE_INDICES[CUBE_FACE_LENGTH] = 
    { 0, 1, 3
    , 3, 1, 2
    , 0, 4, 5
    , 0, 5, 1
    , 1, 5, 6
    , 6, 2, 1
    , 3, 2, 6
    , 3, 6, 7
    , 0, 3, 4
    , 3, 7, 4
    };


// payload stuff
static const SizeT PAYLOAD_VERTEX_COUNT = 41;
static const SizeT PAYLOAD_FACE_COUNT = 72;
static const SizeT PAYLOAD_VERTEX_LENGTH = PAYLOAD_VERTEX_COUNT*3;
static const SizeT PAYLOAD_FACE_LENGTH = PAYLOAD_FACE_COUNT*3;

static FloatT PAYLOAD_VERTICES[PAYLOAD_VERTEX_LENGTH] =
    // top [0, 1)
    {  0.00,  0.00, 0.00 

    // layer 1 [1, 9)
    ,  1.00,  0.00,  1.00
    ,  0.71, -0.71,  1.00
    ,  0.00, -1.00,  1.00
    , -0.71, -0.71,  1.00
    , -1.00,  0.00,  1.00
    , -0.71,  0.71,  1.00
    ,  0.00,  1.00,  1.00
    ,  0.71,  0.71,  1.00

    // layer 2 [9, 17)
    ,  1.83,  0.00,  4.00
    ,  1.29, -1.29,  4.00
    ,  0.00, -1.83,  4.00
    , -1.29, -1.29,  4.00
    , -1.83,  0.00,  4.00
    , -1.29,  1.29,  4.00
    ,  0.00,  1.83,  4.00
    ,  1.29,  1.29,  4.00
    
    // layer 3 [17,25)
    ,  1.50,  0.00,  4.40
    ,  1.06, -1.06,  4.40
    ,  0.00, -1.50,  4.40
    , -1.06, -1.06,  4.40
    , -1.50,  0.00,  4.40
    , -1.06,  1.06,  4.40
    ,  0.00,  1.50,  4.40
    ,  1.06,  1.06,  4.40

    // layer 4 [25,33)
    ,  1.60,  0.00,  4.40
    ,  1.13, -1.13,  4.40
    ,  0.00, -1.60,  4.40
    , -1.13, -1.13,  4.40
    , -1.60,  0.00,  4.40
    , -1.13,  1.13,  4.40
    ,  0.00,  1.60,  4.40
    ,  1.13,  1.13,  4.40

    // layer 5 [33, 41)
    ,  1.60,  0.00,  7.20
    ,  1.13, -1.13,  7.20
    ,  0.00, -1.60,  7.20
    , -1.13, -1.13,  7.20
    , -1.60,  0.00,  7.20
    , -1.13,  1.13,  7.20
    ,  0.00,  1.60,  7.20
    ,  1.13,  1.13,  7.20


    // end of payload vertices
    };

static FloatT PAYLOAD_NORMALS[PAYLOAD_VERTEX_LENGTH] =
    // top [0, 1)
    {  0,  0,  -1

    // layer 1 [1, 9)
    ,  1.00,  0.00,  0.00
    ,  0.71, -0.71,  0.00
    ,  0.00, -1.00,  0.00
    , -0.71, -0.71,  0.00
    , -1.00,  0.00,  0.00
    , -0.71,  0.71,  0.00
    ,  0.00,  1.00,  0.00
    ,  0.71,  0.71,  0.00


    // layer 2 [9, 17)
    ,  1.00,  0.00,  0.00
    ,  0.71, -0.71,  0.00
    ,  0.00, -1.00,  0.00
    , -0.71, -0.71,  0.00
    , -1.00,  0.00,  0.00
    , -0.71,  0.71,  0.00
    ,  0.00,  1.00,  0.00
    ,  0.71,  0.71,  0.00
    
    // layer 3 [17,25)
    ,  1.00,  0.00,  0.00
    ,  0.71, -0.71,  0.00
    ,  0.00, -1.00,  0.00
    , -0.71, -0.71,  0.00
    , -1.00,  0.00,  0.00
    , -0.71,  0.71,  0.00
    ,  0.00,  1.00,  0.00
    ,  0.71,  0.71,  0.00

    // layer 4 [25,33)
    ,  1.00,  0.00,  0.00
    ,  0.71, -0.71,  0.00
    ,  0.00, -1.00,  0.00
    , -0.71, -0.71,  0.00
    , -1.00,  0.00,  0.00
    , -0.71,  0.71,  0.00
    ,  0.00,  1.00,  0.00
    ,  0.71,  0.71,  0.00

    // layer 5 [33, 41)
    ,  1.00,  0.00,  0.00
    ,  0.71, -0.71,  0.00
    ,  0.00, -1.00,  0.00
    , -0.71, -0.71,  0.00
    , -1.00,  0.00,  0.00
    , -0.71,  0.71,  0.00
    ,  0.00,  1.00,  0.00
    ,  0.71,  0.71,  0.00


    // end of payload normals
    };

static IndexT PAYLOAD_INDICES[PAYLOAD_FACE_LENGTH] = 
    // roof [0,8)
    { 0, 1, 2
    , 0, 2, 3
    , 0, 3, 4
    , 0, 4, 5
    , 0, 5, 6
    , 0, 6, 7
    , 0, 7, 8
    , 0, 8, 1

    // layer 1-2 [8,24)
    , 1,9,10,
    1,10,2,
    2,10,11,
    2,11,3,
    3,11,12,
    3,12,4,
    4,12,13,
    4,13,5,
    5,13,14,
    5,14,6,
    6,14,15,
    6,15,7,
    7,15,16,
    7,16,8,
    8,16,9,
    8,9,1

    // layer 2-3 [24, 40)
    ,9,17,18
    ,9,18,10,
    10,18,19,
    10,19,11,
    11,19,20,
    11,20,12,
    12,20,21,
    12,21,13,
    13,21,22,
    13,22,14,
    14,22,23,
    14,23,15,
    15,23,24,
    15,24,16,
    16,24,17,
    16,17,9

    // layer 3-4 [40, 56)
    ,17,25,26,
    17,26,18,
    18,26,27,
    18,27,19,
    19,27,28,
    19,28,20,
    20,28,29,
    20,29,21,
    21,29,30,
    21,30,22,
    22,30,31,
    22,31,23,
    23,31,32,
    23,32,24,
    24,32,25,
    24,25,17

    // layer 4-5 [56,72)
    , 25,33,34,
    25,34,26,
    26,34,35,
    26,35,27,
    27,35,36,
    27,36,28,
    28,36,37,
    28,37,29,
    29,37,38,
    29,38,30,
    30,38,39,
    30,39,31,
    31,39,40,
    31,40,32,
    32,40,33,
    32,33,25

    // end of payload indices
    };


MeshData::MeshData(FloatT* pvertices, FloatT* pnormals, SizeT pvertex_length, IndexT* pindices, SizeT pindex_count)
    : vertices(pvertices), normals(pnormals)
        , indices(pindices), vertex_length(pvertex_length), index_count(pindex_count)
    {}


MeshData payloadMeshData() {
    return MeshData(PAYLOAD_VERTICES, PAYLOAD_NORMALS, PAYLOAD_VERTEX_LENGTH, PAYLOAD_INDICES, PAYLOAD_FACE_LENGTH);
}

MeshData cubeMeshData() {
    return MeshData(CUBE_VERTICES, CUBE_NORMALS, CUBE_VERTEX_LENGTH, CUBE_INDICES, CUBE_FACE_LENGTH);
}

} // namespace RSimView