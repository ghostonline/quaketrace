#pragma once

#include "Math.hpp"
#include "Color.hpp"
#include "Texture.hpp"
#include "Lighting.hpp"
#include "Camera.hpp"
#include <vector>

class FrameBuffer;
struct Ray;

struct Scene
{
    struct Material
    {
        Material() : texture(-1), color(0.0f, 0.0f, 0.0f), flags(NUM_FLAGS) {}

        int texture;
        math::Vec3f u;
        math::Vec3f v;
        math::Vec2f offset;
        Color color;

        // Flags
        enum Flag
        {
            FLAG_SKYSHADER,
            NUM_FLAGS,
        };
        std::vector<bool> flags;

        math::Vec2f positionToUV(const math::Vec3f& pos) const;
    };

    struct Sphere
    {
        math::Vec3f origin;
        float radius;
        Color color;
    };

    struct Plane
    {
        math::Vec3f origin;
        math::Vec3f normal;
        Color color;
    };

    struct Triangle
    {
        math::Vec3f a;
        math::Vec3f b;
        math::Vec3f c;

        Color color;
    };

    struct ConvexPolygon
    {
        static const ConvexPolygon create(const std::vector<math::Vec3f>& vertices, const math::Vec3f& normal, const Material& material);

        std::vector<math::Vec3f> vertices;

        // Precalculated data based on vertices
        struct Plane { math::Vec3f normal; math::Vec3f origin; };

        Plane plane;

        std::vector<math::Vec3f> edgeNormals;
        std::vector<Plane> edgePlanes;

        Material material;

        // Flags
        enum Flag
        {
            FLAG_TWOSIDED,
            FLAG_SHADOWCAST,
            NUM_FLAGS,
        };
        std::vector<bool> flags;

    private:
        ConvexPolygon() : flags(NUM_FLAGS) {}
    };

    std::vector<Camera> cameras;
    std::vector<Sphere> spheres;
    std::vector<Plane> planes;
    std::vector<Triangle> triangles;
    std::vector<ConvexPolygon> polygons;
    Lighting lighting;

    struct TextureData
    {
        Texture texture;
        std::vector<bool> fullbright;
    };
    std::vector<TextureData> textures;

    struct TexturePixel
    {
        Color color;
        bool fullbright;
    };
    TexturePixel getTexturePixel(const Material& mat, const math::Vec3f& pos) const;
    TexturePixel getSkyPixel(const Material& mat, const Ray& ray, const Camera& camera, const math::Vec2i& screen) const;

    static void initDefault(Scene* scene);
    static Scene createShadowScene(const Scene& scene);
    static Scene cullGeometry(const Scene& scene, const Camera& camera);
};

inline math::Vec2f Scene::Material::positionToUV(const math::Vec3f& pos) const
{
    math::Vec2f uv;
    uv.x = math::dot(pos, u);
    uv.y = math::dot(pos, v);
    uv += offset;
    return uv;
}
