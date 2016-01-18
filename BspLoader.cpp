#include "BspLoader.hpp"
#include "Logger.hpp"
#include <cstdint>

using namespace std;

namespace {
    enum Lumps
    {
        LUMP_ENTITIES,
        LUMP_PLANES,
        LUMP_MIPTEX,
        LUMP_VERTICES,
        LUMP_VISILIST,
        LUMP_NODES,
        LUMP_TEXINFO,
        LUMP_FACES,
        LUMP_LIGHTMAPS,
        LUMP_CLIPNODES,
        LUMP_LEAVES,
        LUMP_LFACE,
        LUMP_EDGES,
        LUMP_LEDGES,
        LUMP_MODELS,
        NUM_LUMPS,
    };

    struct Entry
    {
        int32_t offset;
        int32_t size;
    };

    struct Header
    {
        int32_t version;
        Entry lumps[NUM_LUMPS];
    };

    typedef float scalar_t;

    struct Vec3f
    {
        scalar_t x;
        scalar_t y;
        scalar_t z;
    };

    struct BoundingBox
    {
        Vec3f min;
        Vec3f max;
    };

    struct Model
    {
        BoundingBox bound;
        Vec3f origin;
        int32_t node_id0;   // index of first BSP node
        int32_t node_id1;   // index of the first Clip node
        int32_t node_id2;   // index of the second Clip node
        int32_t node_id3;   // usually zero
        int32_t numleafs;   // number of BSP leaves
        int32_t face_id;    // index of Faces
        int32_t face_num;   // number of Faces
    };

    struct Vertex
    {
        scalar_t x;
        scalar_t y;
        scalar_t z;
    };

    struct Face
    {
        uint16_t plane_id;   // The plane in which the face lies
        uint16_t side;       // 0 if in front of the plane, 1 if behind the plane
        int32_t ledge_id;    // first edge in the List of edges
        uint16_t ledge_num;  // number of edges in the List of edges
        uint16_t texinfo_id; // index of the Texture info the face is part of
        uint8_t typelight;   // type of lighting, for the face
        uint8_t baselight;   // from 0xFF (dark) to 0 (bright)
        uint8_t light[2];    // two additional light models
        int32_t lightmap;    // Pointer inside the general light map, or -1
    };

    struct Plane
    {
        Vec3f normal;
        scalar_t dist; // Offset to plane, along the normal vector.
        int32_t type;
    };

    struct Edge
    {
        uint16_t vertex_idx_start;
        uint16_t vertex_idx_end;
    };

    template<typename T>
    inline const T& grab(const void* data, int offset = 0)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
        return *reinterpret_cast<const T*>(bytes + offset);
    }
}

const Scene BspLoader::createSceneFromBsp(const void* data, int size)
{
    Scene scene;
    Scene::initDefault(&scene);

    const Header& header = grab<Header>(data);
    const char* entities = &grab<char>(data, header.lumps[LUMP_ENTITIES].offset);
    const Face* faces = &grab<Face>(data, header.lumps[LUMP_FACES].offset);
    int numFaces = header.lumps[LUMP_FACES].size / sizeof(Face);
    const Vertex* vertices = &grab<Vertex>(data, header.lumps[LUMP_VERTICES].offset);
    int numVertices = header.lumps[LUMP_VERTICES].size / sizeof(Vertex);
    const Edge* edges = &grab<Edge>(data, header.lumps[LUMP_EDGES].offset);
    int numEdges = header.lumps[LUMP_EDGES].size / sizeof(Edge);

    for (int ii = 0; ii < numFaces; ++ii)
    {
        const auto& face = faces[ii];
        LOG("plane: %d", face.plane_id);
    }

    return scene;
}
