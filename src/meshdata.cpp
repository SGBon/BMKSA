#include "meshdata.hpp"

namespace RSimView {
static const SizeT PAYLOAD_VERTEX_COUNT = 9;
static const SizeT PAYLOAD_INDEX_COUNT = 9;

static FloatT PAYLOAD_VERTICES[PAYLOAD_VERTEX_COUNT] = {1,0,0, 0,1,0, 0,0,1};
static FloatT PAYLOAD_NORMALS[PAYLOAD_VERTEX_COUNT] = {1,0,0, 0,1,0, 0,0,1};
static IndexT PAYLOAD_INDICES[PAYLOAD_INDEX_COUNT] = {0,1,2};


MeshData::MeshData(FloatT* pvertices, FloatT* pnormals, SizeT pvertex_count, IndexT* pindices, SizeT pindex_count)
    : vertices(pvertices), normals(pnormals), vertex_count(pvertex_count)
        , indices(pindices), index_count(pindex_count)
    {}


MeshData payloadMeshData() {
    return MeshData(PAYLOAD_VERTICES, PAYLOAD_NORMALS, PAYLOAD_VERTEX_COUNT, PAYLOAD_INDICES, PAYLOAD_INDEX_COUNT);
}

} // namespace RSimView