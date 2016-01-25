#include "BspLoader.hpp"
#include "Logger.hpp"
#include "Assert.hpp"
#include "Util.hpp"
#include <cstdint>
#include <cstdlib>

using namespace std;

namespace {
    enum Lumps
    {
        LUMP_ENTITIES,
        LUMP_PLANES,
        LUMP_TEXTURES,
        LUMP_VERTEXES,
        LUMP_VISIBILITY,
        LUMP_NODES,
        LUMP_TEXINFO,
        LUMP_FACES,
        LUMP_LIGHTING,
        LUMP_CLIPNODES,
        LUMP_LEAFS,
        LUMP_MARKSURFACES,
        LUMP_EDGES,
        LUMP_SURFEDGES,
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

    struct Vec
    {
        scalar_t x;
        scalar_t y;
        scalar_t z;
    };

    struct BoundingBox
    {
        Vec min;
        Vec max;
    };

    struct Model
    {
        BoundingBox bound;
        Vec origin;
        int32_t node_bsp_id;
        int32_t node_clip_first;
        int32_t node_clip_second;
        int32_t node_unused;
        int32_t numleafs;   // number of BSP leaves
        int32_t face_id;    // index of Faces
        int32_t face_num;   // number of Faces
    };

    struct Face
    {
        int16_t plane_id;   // The plane in which the face lies
        int16_t side;       // 0 if in front of the plane, 1 if behind the plane
        int32_t ledge_id;    // first edge in the List of edges
        int16_t ledge_num;  // number of edges in the List of edges
        int16_t texinfo_id; // index of the Texture info the face is part of
        uint8_t typelight;   // type of lighting, for the face
        uint8_t baselight;   // from 0xFF (dark) to 0 (bright)
        uint8_t light[2];    // two additional light models
        int32_t lightmap;    // Pointer inside the general light map, or -1
    };

    struct Plane
    {
        Vec normal;
        scalar_t dist; // Offset to plane, along the normal vector.
        int32_t type;
    };

    struct Edge
    {
        uint16_t vertex_idx_start;
        uint16_t vertex_idx_end;
    };

    typedef Vec Vertex;

    template<typename T>
    struct Lump
    {
        int size;
        const T* array;

        inline const T& operator[](int idx) const
        {
            ASSERT(0 <= idx && idx < size);
            return array[idx];
        }

        static inline const Lump fromEntry(const void* data, const Entry& entry)
        {
            Lump l;
            l.size = entry.size / sizeof(T);
            ASSERT(l.size > 0);
            l.array = util::castFromMemory<T>(data, entry.offset);
            return l;
        }
    };

    static const Color DISTINCT_COLORS[] = {
        {0xD5/255.0f, 0x6D/255.0f, 0x73/255.0f},
        {0x6A/255.0f, 0xD1/255.0f, 0x45/255.0f},
        {0x7C/255.0f, 0xCC/255.0f, 0xD3/255.0f},
        {0x75/255.0f, 0x6C/255.0f, 0xCE/255.0f},
        {0xD7/255.0f, 0xA8/255.0f, 0x35/255.0f},
        {0x42/255.0f, 0x63/255.0f, 0x27/255.0f},
        {0x3A/255.0f, 0x2E/255.0f, 0x2D/255.0f},
        {0xD0/255.0f, 0xD2/255.0f, 0x9C/255.0f},
        {0x7B/255.0f, 0x31/255.0f, 0x6B/255.0f},
        {0xCC/255.0f, 0x52/255.0f, 0xCF/255.0f},
        {0xD4/255.0f, 0x40/255.0f, 0x2F/255.0f},
        {0xCC/255.0f, 0x8A/255.0f, 0xC3/255.0f},
        {0x6B/255.0f, 0x93/255.0f, 0xC5/255.0f},
        {0x77/255.0f, 0x51/255.0f, 0x28/255.0f},
        {0x52/255.0f, 0x7B/255.0f, 0x6C/255.0f},
        {0xC7/255.0f, 0xD7/255.0f, 0x49/255.0f},
        {0x76/255.0f, 0xC9/255.0f, 0x79/255.0f},
        {0xD8/255.0f, 0x41/255.0f, 0x83/255.0f},
        {0xD1/255.0f, 0x78/255.0f, 0x3A/255.0f},
        {0xA9/255.0f, 0x84/255.0f, 0x7E/255.0f},
        {0x46/255.0f, 0x44/255.0f, 0x6B/255.0f},
        {0x7D/255.0f, 0x2A/255.0f, 0x2C/255.0f},
        {0xCC/255.0f, 0xC5/255.0f, 0xCF/255.0f},
        {0x68/255.0f, 0xD7/255.0f, 0xB4/255.0f},
        {0xA6/255.0f, 0x9A/255.0f, 0x4E/255.0f},
    };
    static const int DISTINCT_COLOR_COUNT = UTIL_ARRAY_SIZE(DISTINCT_COLORS);
}

const Scene BspLoader::createSceneFromBsp(const void* data, int size)
{
    Scene scene;
    //Scene::initDefault(&scene);

    const Header& header = *util::castFromMemory<Header>(data);
    //const char* entities = &grab<char>(data, header.lumps[LUMP_ENTITIES].offset);
    const Lump<Face> faces = Lump<Face>::fromEntry(data, header.lumps[LUMP_FACES]);
    const Lump<Vertex> vertices = Lump<Vertex>::fromEntry(data, header.lumps[LUMP_VERTEXES]);
    const Lump<Edge> edges = Lump<Edge>::fromEntry(data, header.lumps[LUMP_EDGES]);
    const Lump<Model> models = Lump<Model>::fromEntry(data, header.lumps[LUMP_MODELS]);
    const Lump<int16_t> edgeIndices = Lump<int16_t>::fromEntry(data, header.lumps[LUMP_SURFEDGES]);
    const Lump<uint16_t> faceIndices = Lump<uint16_t>::fromEntry(data, header.lumps[LUMP_MARKSURFACES]);

    const Model& base = models[0];
    for (int ii = 0; ii < base.face_num; ++ii)
    {
        const int faceLookup = faceIndices[base.face_id + ii];
        const Face& f = faces[faceLookup];
        printf("face: %d\n", faceLookup);
        for (int jj = 0; jj < f.ledge_num; ++jj)
        {
            const int edgeLookup = std::abs(edgeIndices[f.ledge_id + jj]);
            const Edge& e = edges[edgeLookup];

            const Vertex& a = vertices[e.vertex_idx_start];
            const Vertex& b = vertices[e.vertex_idx_end];
            printf("%d -> %d\n", e.vertex_idx_start, e.vertex_idx_end);
            //printf("(%f, %f, %f) -> (%f, %f, %f)\n", a.x, a.y, a.z, b.x, b.y, b.z);
        }
        /*
        const auto& vertex = vertices[ii];
        const Color& color = DISTINCT_COLORS[ii % DISTINCT_COLOR_COUNT];
        scene.spheres.push_back({math::Vec3f(vertex.x, vertex.y, vertex.z), 10.0f, color});
         */
    }

    auto& cam = scene.camera;
    cam.origin = {-144, 0, -72};
    cam.near = 0.0f;
    cam.far = 100000.0f;
    cam.direction = {0, 1, 0};
    cam.right = {1, 0, 0};
    cam.up = {0, 0, 1};

    const float fov = 60;
    cam.halfViewAngles.set(
         std::tan(math::deg2rad(fov)) / 2.0f,
         std::tan(math::deg2rad(fov)) / 2.0f
    );

    Scene::pointCamera(&scene.camera, {-144, 0, -72}, {0, 1, 0}, {0, 0, 1});
    scene.ambientLightFactor = 1.0f;

    return scene;
}
