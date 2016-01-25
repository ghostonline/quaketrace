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
    /* FIXME - put this typedef elsewhere */
    typedef uint8_t byte;

#define BSPVERSION     29
#define BSP2RMQVERSION (('B' << 24) | ('S' << 16) | ('P' << 8) | '2')
#define BSP2VERSION    ('B' | ('S' << 8) | ('P' << 16) | ('2' << 24))

    typedef struct {
        int32_t fileofs;
        int32_t filelen;
    } lump_t;

    typedef struct {
        float mins[3], maxs[3];
        float origin[3];
        int32_t headnode[4];	/* 4 for backward compat, only 3 hulls exist */
        int32_t visleafs;		/* not including the solid leaf 0 */
        int32_t firstface, numfaces;
    } dmodel_t;

    typedef struct {
        int32_t version;
        lump_t lumps[NUM_LUMPS];
    } dheader_t;

    typedef struct {
        int32_t nummiptex;
        int32_t dataofs[];		/* [nummiptex] */
    } dmiptexlump_t;

    typedef char miptex_t[16];

    typedef struct {
        float point[3];
    } dvertex_t;

    typedef struct {
        float normal[3];
        float dist;
        int32_t type;
    } dplane_t;

    typedef struct {
        int32_t planenum;
        int16_t children[2];	/* negative numbers are -(leafs+1), not nodes */
        int16_t mins[3];		/* for sphere culling */
        int16_t maxs[3];
        uint16_t firstface;
        uint16_t numfaces;		/* counting both sides */
    } bsp29_dnode_t;

    typedef struct {
        int32_t planenum;
        int32_t children[2];	/* negative numbers are -(leafs+1), not nodes */
        int16_t mins[3];		/* for sphere culling */
        int16_t maxs[3];
        uint32_t firstface;
        uint32_t numfaces;		/* counting both sides */
    } bsp2rmq_dnode_t;

    typedef struct {
        int32_t planenum;
        int32_t children[2];	/* negative numbers are -(leafs+1), not nodes */
        float mins[3];		/* for sphere culling */
        float maxs[3];
        uint32_t firstface;
        uint32_t numfaces;		/* counting both sides */
    } bsp2_dnode_t;

    /*
     * Note that children are interpreted as unsigned values now, so that we can
     * handle > 32k clipnodes. Values > 0xFFF0 can be assumed to be CONTENTS
     * values and can be read as the signed value to be compatible with the above
     * (i.e. simply subtract 65536).
     */
    typedef struct {
        int32_t planenum;
        uint16_t children[2];	/* "negative" numbers are contents */
    } bsp29_dclipnode_t;

    typedef struct {
        int32_t planenum;
        int32_t children[2];	/* negative numbers are contents */
    } bsp2_dclipnode_t;

    typedef struct texinfo_s {
        float vecs[2][4];		/* [s/t][xyz offset] */
        int32_t miptex;
        int32_t flags;
    } texinfo_t;

    /*
     * Note that edge 0 is never used, because negative edge nums are used for
     * counterclockwise use of the edge in a face.
     */
    typedef struct {
        uint16_t v[2];		/* vertex numbers */
    } bsp29_dedge_t;

    typedef struct {
        uint32_t v[2];		/* vertex numbers */
    } bsp2_dedge_t;

#define MAXLIGHTMAPS 4
    typedef struct {
        int16_t planenum;
        int16_t side;

        int32_t firstedge;		/* we must support > 64k edges */
        int16_t numedges;
        int16_t texinfo;

        /* lighting info */
        uint8_t styles[MAXLIGHTMAPS];
        int32_t lightofs;		/* start of [numstyles*surfsize] samples */
    } bsp29_dface_t;

    typedef struct {
        int32_t planenum;
        int32_t side;

        int32_t firstedge;		/* we must support > 64k edges */
        int32_t numedges;
        int32_t texinfo;

        /* lighting info */
        uint8_t styles[MAXLIGHTMAPS];
        int32_t lightofs;		/* start of [numstyles*surfsize] samples */
    } bsp2_dface_t;

    /* Ambient sounds */
#define AMBIENT_WATER   0
#define AMBIENT_SKY     1
#define AMBIENT_SLIME   2
#define AMBIENT_LAVA    3
#define NUM_AMBIENTS    4

    /*
     * leaf 0 is the generic CONTENTS_SOLID leaf, used for all solid areas
     * all other leafs need visibility info
     */
    typedef struct {
        int32_t contents;
        int32_t visofs;		/* -1 = no visibility info */
        int16_t mins[3];		/* for frustum culling */
        int16_t maxs[3];
        uint16_t firstmarksurface;
        uint16_t nummarksurfaces;
        uint8_t ambient_level[NUM_AMBIENTS];
    } bsp29_dleaf_t;
    
    typedef struct {
        int32_t contents;
        int32_t visofs;		/* -1 = no visibility info */
        int16_t mins[3];		/* for frustum culling */
        int16_t maxs[3];
        uint32_t firstmarksurface;
        uint32_t nummarksurfaces;
        uint8_t ambient_level[NUM_AMBIENTS];
    } bsp2rmq_dleaf_t;
    
    typedef struct {
        int32_t contents;
        int32_t visofs;		/* -1 = no visibility info */
        float mins[3];		/* for frustum culling */
        float maxs[3];
        uint32_t firstmarksurface;
        uint32_t nummarksurfaces;
        uint8_t ambient_level[NUM_AMBIENTS];
    } bsp2_dleaf_t;

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

        static inline const Lump fromEntry(const void* data, const lump_t& entry)
        {
            Lump l;
            l.size = entry.filelen / sizeof(T);
            ASSERT(l.size > 0);
            l.array = util::castFromMemory<T>(data, entry.fileofs);
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

    auto& header = *util::castFromMemory<dheader_t>(data);
    //const char* entities = &grab<char>(data, header.lumps[LUMP_ENTITIES].offset);
    auto faces = Lump<bsp29_dface_t>::fromEntry(data, header.lumps[LUMP_FACES]);
    auto vertices = Lump<dvertex_t>::fromEntry(data, header.lumps[LUMP_VERTEXES]);
    auto edges = Lump<bsp29_dedge_t>::fromEntry(data, header.lumps[LUMP_EDGES]);
    auto models = Lump<dmodel_t>::fromEntry(data, header.lumps[LUMP_MODELS]);
    auto edgeIndices = Lump<int16_t>::fromEntry(data, header.lumps[LUMP_SURFEDGES]);
    auto faceIndices = Lump<uint16_t>::fromEntry(data, header.lumps[LUMP_MARKSURFACES]);

    auto& base = models[0];
    for (int ii = 0; ii < base.numfaces; ++ii)
    {
        const int flIdx = base.firstface + ii;
        const int faceLookup = faceIndices[flIdx];
        auto& f = faces[faceLookup];
        printf("face: %d\n", faceLookup);
        for (int jj = 0; jj < f.numedges; ++jj)
        {
            const int elIdx = f.firstedge + jj;
            const int edgeLookup = std::abs(edgeIndices[elIdx]);
            ASSERT(edgeLookup != 0);
            auto& e = edges[edgeLookup];

            auto& a = vertices[e.v[0]];
            auto& b = vertices[e.v[1]];
            printf("%d -> %d\n", e.v[0], e.v[1]);
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
