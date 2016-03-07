#include "BspLoader.hpp"
#include "Logger.hpp"
#include "Assert.hpp"
#include "Util.hpp"
#include "Texture.hpp"
#include "AssetHelper.hpp"
#include "BspEntity.hpp"
#include "ArrayView.hpp"
#include <cstdint>
#include <cstdlib>

using namespace std;

#define COLOR_DEBUG 0

namespace {
    static const float PLAYER_EYE_HEIGHT = 56.0f;
    static const float LIGHT_SOURCE_RADIUS = 16;

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
        LUMP_LEAVES,
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

    struct BoundingBoxCompressed // Bounds rounded to the nearest 16 units
    {
        int16_t min[3];
        int16_t max[3];
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

    struct BspNode
    {
        int32_t plane_id;
        int16_t children[2]; // bit15==1 for leaves (rendering), bit15==0 for nodes (collision)
                              // id-1 to obtain the leaf index
        BoundingBoxCompressed bounds;
        uint16_t face_id;
        uint16_t face_num;
    };

    struct BspLeaf {
        int32_t type;               // -2 for solid leaf (always idx == 0)
        int32_t vis_id;             // -1 for no vis info
        BoundingBoxCompressed bounds;
        uint16_t lface_id;
        uint16_t lface_num;
        uint8_t ambient_level[4]; // ambient sfx
    };

    struct MipsTexture
    {
        enum
        {
            MIP_1X1,
            MIP_2X2,
            MIP_4X4,
            MIP_8X8,
            NUM_MIPS,
        };

        char name[16];
        uint32_t width;
        uint32_t height;
        uint32_t offset[NUM_MIPS];
    };

    struct TextureInfo
    {
        Vec u;
        scalar_t offset_u;
        Vec v;
        scalar_t offset_v;
        uint32_t   texture_id;         // Index of Mip Texture
        uint32_t   animated;           // 0 for ordinary textures, 1 for water
    };

#if COLOR_DEBUG
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
#endif

    inline math::Vec3f vert2vec3(const Vertex& vert)
    {
        return { vert.x, vert.y, vert.z };
    }

    inline math::Vec3f norm2vec3(const Vec& normal)
    {
        return { normal.x, normal.y, normal.z };
    }

    inline Scene::Material info2material(const TextureInfo& info, const Color& c)
    {
        Scene::Material mat;
        mat.color = c;
        mat.u = vert2vec3(info.u);
        mat.v = vert2vec3(info.v);
        mat.texture = info.texture_id;
        mat.offset.x = info.offset_u;
        mat.offset.y = info.offset_v;
        return mat;
    }

    inline bool isFullBright(int index)
    {
        return index >= 224;
    }

    template<typename T>
    static inline const util::ArrayView<T> entry2view(const void* data, const Entry& entry)
    {
        int size = entry.size / sizeof(T);
        ASSERT(size > 0);
        auto array = util::castFromMemory<T>(data, entry.offset);
        return {array, size};
    }

    static inline bool isSkyTexture(const char* name)
    {
        return !std::strncmp(name, "sky", 3);
    }

    static inline float getNormalizedLightValue(const BspEntity& entity)
    {
        float value = 0.0f;
        if (entity.hasProperty(BspEntity::Property::KEY_LIGHT))
        {
            value = entity.getProperty(BspEntity::Property::KEY_LIGHT).number / 255.0f;
        }
        return value;
    }
}

const Scene BspLoader::createSceneFromBsp(const void* data, int size)
{
    printBspAsObj(data, size);

    Scene scene;

    auto& header = *util::castFromMemory<Header>(data);
    auto planes = entry2view<Plane>(data, header.lumps[LUMP_PLANES]);
    auto faces = entry2view<Face>(data, header.lumps[LUMP_FACES]);
    auto vertices = entry2view<Vertex>(data, header.lumps[LUMP_VERTEXES]);
    auto edges = entry2view<Edge>(data, header.lumps[LUMP_EDGES]);
    auto models = entry2view<Model>(data, header.lumps[LUMP_MODELS]);
    auto edgeIndices = entry2view<int32_t>(data, header.lumps[LUMP_SURFEDGES]);
    auto textureInfo = entry2view<TextureInfo>(data, header.lumps[LUMP_TEXINFO]);
    auto entitiesEntry = entry2view<char>(data, header.lumps[LUMP_ENTITIES]);
    auto entities = BspEntity::parseList({entitiesEntry.array, entitiesEntry.size});

    std::vector<CameraDefinition> cameras;
    std::vector<int> modelIndices;
    CameraDefinition startCamera;
    for (int ii = 0; ii < static_cast<int>(entities.size()); ++ii)
    {
        const BspEntity& entity = entities[ii];
        switch (entity.type)
        {
            case BspEntity::TYPE_PLAYER_START:
                startCamera = parsePlayerStart(entity);
                break;
            case BspEntity::TYPE_INTERMISSION_CAMERA:
                {
                    const auto camera = parseIntermissionCamera(entity);
                    cameras.push_back(camera);
                }
                break;
            case BspEntity::TYPE_LIGHT:
                {
                    const auto light = parseLight(entity);
                    scene.lighting.add(light);
                }
                break;
            case BspEntity::TYPE_WORLD_SPAWN:
                scene.lighting.ambient = getNormalizedLightValue(entity);
                break;
            default:
                break;
        }

        if (entity.hasProperty(BspEntity::Property::KEY_MODEL))
        {
            modelIndices.push_back(entity.getProperty(BspEntity::Property::KEY_MODEL).integer);
        }
    }

    modelIndices.push_back(0);

    if (!cameras.size())
    {
        cameras.push_back(startCamera);
    }

    const void* palette = AssetHelper::getRaw(AssetHelper::PALETTE, nullptr);
    const int32_t* textureOffsets = util::castFromMemory<int32_t>(data, header.lumps[LUMP_TEXTURES].offset);
    int textureCount = textureOffsets[0];
    scene.textures.reserve(textureCount);
    for (int ii = 1; ii <= textureCount; ++ii)
    {
        int offset = header.lumps[LUMP_TEXTURES].offset + textureOffsets[ii];
        auto def = util::castFromMemory<MipsTexture>(data, offset);
        auto indices = util::castFromMemory<uint8_t>(data, offset + def->offset[MipsTexture::MIP_1X1]);
        std::vector<bool> fullbright(def->width * def->height);
        const bool isSky = isSkyTexture(def->name);
        for (int ii = util::lastIndex(fullbright); ii >= 0; --ii)
        {
            fullbright[ii] = isFullBright(indices[ii]) || isSky;
        }
        scene.textures.push_back({Texture::createFromIndexedRGB(def->width, def->height, indices, palette), fullbright});
    }

    for (int ii = util::lastIndex(modelIndices); ii >= 0; --ii)
    {
        const int modelIdx = modelIndices[ii];
        const Model& model = models[modelIdx];

        for (int ii = 0; ii < model.face_num; ++ii)
        {
            const int faceIdx = model.face_id + ii;
            const Face& f = faces[faceIdx];
            std::vector<math::Vec3f> polyVertices(f.ledge_num);
            for (int jj = 0; jj < f.ledge_num; ++jj)
            {
                const int edgeLookup = edgeIndices[f.ledge_id + jj];
                ASSERT(edgeLookup != 0);
                const Edge& e = edges[std::abs(edgeLookup)];

                int idxStart = edgeLookup > 0 ? e.vertex_idx_start : e.vertex_idx_end;
                polyVertices[jj] = vert2vec3(vertices[idxStart]);
            }
            const Plane& plane = planes[f.plane_id];
            const auto normal = norm2vec3(plane.normal) * static_cast<float>(1 - f.side * 2);
            const auto& texture = textureInfo[f.texinfo_id];
#if COLOR_DEBUG
            auto mat = info2material(texture, DISTINCT_COLORS[ii % DISTINCT_COLOR_COUNT]);
#else
            auto mat = info2material(texture, Color(0.0f, 0.0f, 0.0f));
#endif
            const auto poly = Scene::ConvexPolygon::create(polyVertices, normal, mat);
            scene.polygons.push_back(poly);
        }
    }

    const float fov = 60;
    const auto& camera = cameras.front();
    Scene::pointCamera(&scene.camera, camera.origin, camera.direction, {0, 0, 1});
    scene.camera.halfViewAngles.set(
                                     std::tan(math::deg2rad(fov)) / 2.0f,
                                     std::tan(math::deg2rad(fov)) / 2.0f
                                     );

    return scene;
}

const BspLoader::CameraDefinition BspLoader::parseIntermissionCamera(const BspEntity& entity)
{
    static const math::Vec3f UNIT_X{1.0f, 0.0f, 0.0f};
    static const math::Vec3f UNIT_Y{0.0f, 1.0f, 0.0f};
    static const math::Vec3f UNIT_Z{0.0f, 0.0f, 1.0f};

    ASSERT(entity.type == BspEntity::TYPE_INTERMISSION_CAMERA);

    const auto& origin = entity.getProperty(BspEntity::Property::KEY_ORIGIN).vec;
    const auto& mangle = entity.getProperty(BspEntity::Property::KEY_MANGLE).vec;

    const float pitch = math::deg2rad(mangle.x);
    const float yaw = math::deg2rad(mangle.y);

    const auto rotZ = math::createRotationMatrix(UNIT_Z, yaw);
    const auto rotY = math::createRotationMatrix(UNIT_Y, pitch);
    const auto direction = rotZ * rotY * UNIT_X;
    return {origin, direction};
}

const BspLoader::CameraDefinition BspLoader::parsePlayerStart(const BspEntity& entity)
{
    static const math::Vec3f UNIT_X{1.0f, 0.0f, 0.0f};
    static const math::Vec3f UNIT_Z{0.0f, 0.0f, 1.0f};
    static const math::Vec3f PLAYER_EYE_OFFSET{0.0f, 0.0f, PLAYER_EYE_HEIGHT};

    ASSERT(entity.type == BspEntity::TYPE_PLAYER_START);

    const auto& origin = entity.getProperty(BspEntity::Property::KEY_ORIGIN).vec;
    const auto& angle = entity.getProperty(BspEntity::Property::KEY_ANGLE).number;

    const float yaw = math::deg2rad(angle);

    const auto rotZ = math::createRotationMatrix(UNIT_Z, yaw);
    const auto direction = rotZ * UNIT_X;
    return {origin + PLAYER_EYE_OFFSET, direction};
}

void BspLoader::printBspAsObj(const void* data, int size)
{
    auto& header = *util::castFromMemory<Header>(data);
    auto faces = entry2view<Face>(data, header.lumps[LUMP_FACES]);
    auto vertices = entry2view<Vertex>(data, header.lumps[LUMP_VERTEXES]);
    auto edges = entry2view<Edge>(data, header.lumps[LUMP_EDGES]);
    auto models = entry2view<Model>(data, header.lumps[LUMP_MODELS]);
    auto edgeIndices = entry2view<int32_t>(data, header.lumps[LUMP_SURFEDGES]);

    const Model& base = models[0];

    for (int ii = 0; ii < vertices.size; ++ii)
    {
        const auto a = vert2vec3(vertices[ii]);
        printf("v %.01f %.01f %.01f\n", a.x, a.y, a.z);
    }

    for (int ii = 0; ii < base.face_num; ++ii)
    {
        const int faceIdx = base.face_id + ii;
        const Face& f = faces[faceIdx];
        printf("f");
        for (int jj = 0; jj < f.ledge_num; ++jj)
        {
            const int edgeLookup = edgeIndices[f.ledge_id + jj];
            ASSERT(edgeLookup != 0);
            const Edge& e = edges[std::abs(edgeLookup)];

            int idxStart = edgeLookup > 0 ? e.vertex_idx_start : e.vertex_idx_end;
            printf(" %d", idxStart + 1);
        }
        printf("\n");
    }
}

const Lighting::Point BspLoader::parseLight(const BspEntity& entity)
{
    ASSERT(entity.type == BspEntity::TYPE_LIGHT);
    auto origin = entity.getProperty(BspEntity::Property::KEY_ORIGIN).vec;
    auto strength = entity.getProperty(BspEntity::Property::KEY_LIGHT).number;
    Lighting::Point light(origin, strength, LIGHT_SOURCE_RADIUS);
    return light;
}
