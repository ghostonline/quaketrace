#pragma once

#include "Math.hpp"
#include "Color.hpp"
#include "Texture.hpp"
#include "Lighting.hpp"
#include <vector>

class FrameBuffer;

struct Scene
{
    struct Material
    {
        Material() : texture(-1), color(0.0f, 0.0f, 0.0f) {}

        int texture;
        math::Vec3f u;
        math::Vec3f v;
        math::Vec2f offset;
        Color color;

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

    private:
        ConvexPolygon() {}
    };

    struct Camera
    {
        math::Vec3f origin;
        math::Vec3f direction;
        math::Vec3f up;
        math::Vec3f right;
        math::Vec2f halfViewAngles;

        float near;
        float far;
    };

    Camera camera;
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

    static void initDefault(Scene* scene);
    static void pointCamera(Camera* camera, const math::Vec3f& pos, const math::Vec3f& forward, const math::Vec3f& up);
    static void pointCameraAt(Camera* camera, const math::Vec3f& pos, const math::Vec3f& focus, const math::Vec3f& up);
};

inline void Scene::pointCameraAt(Camera* camera, const math::Vec3f& pos, const math::Vec3f& focus, const math::Vec3f& up)
{
    pointCamera(camera, pos, math::normalized(focus - pos), up);
}

inline math::Vec2f Scene::Material::positionToUV(const math::Vec3f& pos) const
{
    math::Vec2f uv;
    uv.x = math::dot(pos, u);
    uv.y = math::dot(pos, v);
    uv += offset;
    return uv;
}
