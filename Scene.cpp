#include "Scene.hpp"
#include "Assert.hpp"
#include <cmath>
#include "Math.hpp"
#include "Util.hpp"
#include "Ray.hpp"
#include "Collision3D.hpp"

namespace {
    inline int normalize(float value, int max)
    {
        int normalized = static_cast<int>(std::round(value)) % max;
        if (normalized < 0) { normalized += max; }
        return normalized;
    }
}


void Scene::initDefault(Scene* scene)
{
    ASSERT(scene != nullptr);

    scene->cameras.resize(1);
    scene->cameras[0].setPosition({-30, 0, 0}, {1, 0, 0}, {0, 0, 1});
    const float fov = 60;
    scene->cameras[0].halfViewAngles.set(
                               std::tan(math::deg2rad(fov)) / 2.0f,
                               std::tan(math::deg2rad(fov)) / 2.0f
                               );
    // Spheres
    scene->spheres.push_back({ math::Vec3f(20, 15, 15), 8.0, Color(1.0f, 0.0f, 0.0f) });
    scene->spheres.push_back({ math::Vec3f(5, 15, 5), 6.0, Color(0.0f, 1.0f, 0.0f) });

    // Ground plane
    scene->planes.push_back({ math::Vec3f(0, 0, -10), math::Vec3f(0, 0, 1), Color(0.0f, 0.0f, 1.0f) });

    // Random triangle
    scene->triangles.push_back({ math::Vec3f(0, -5, 0), math::Vec3f(0, 5, 0), math::Vec3f(0, 0, 5), Color(0.5f, 0.5f, 1.0f) });

    // Polygon
    {
        std::vector<math::Vec3f> polyVerts;
        polyVerts.push_back(math::Vec3f(1, -3, 7));
        polyVerts.push_back(math::Vec3f(1, 0, 5));
        polyVerts.push_back(math::Vec3f(1, 3, 7));
        polyVerts.push_back(math::Vec3f(1, 2, 10));
        polyVerts.push_back(math::Vec3f(1, -2, 10));
        auto edgeA = polyVerts[1] - polyVerts[0];
        auto edgeB = polyVerts.back() - polyVerts[0];
        auto normal = math::normalized(math::cross(edgeB, edgeA));
        Material mat;
        mat.color = Color(1.0f, 0.5f, 0.5f);
        mat.texture = -1;
        auto polygon = ConvexPolygon::create(polyVerts, normal, mat);
        scene->polygons.push_back(polygon);
    }

    // Directional lights
    scene->lighting.add({ math::Vec3f(0, 0, -1), 0.3f});

    scene->lighting.ambient = 0.3f;
}

const Scene::ConvexPolygon Scene::ConvexPolygon::create(const std::vector<math::Vec3f>& vertices, const math::Vec3f& normal, const Material& material)
{
    ASSERT(vertices.size() > 2);

    ConvexPolygon poly;
    poly.material = material;
    poly.plane.normal = normal;
    poly.plane.origin = vertices[0];

    // Create edge normals
    poly.edgeNormals.resize(vertices.size());
    for (int ii = util::lastIndex(poly.edgeNormals) - 1; ii >= 0; --ii)
    {
        poly.edgeNormals[ii] = vertices[ii + 1] - vertices[ii];
        math::normalize(&poly.edgeNormals[ii]);
    }
    // Special case: last normal wraps around
    poly.edgeNormals.back() = vertices.front() - vertices.back();
    math::normalize(&poly.edgeNormals.back());

    // Create edge/poly planes
    poly.edgePlanes.resize(poly.edgeNormals.size());
    for (int ii = util::lastIndex(poly.edgeNormals); ii >= 0; --ii)
    {
        const math::Vec3f& edgeNormal = poly.edgeNormals[ii];
        poly.edgePlanes[ii].normal = math::normalized(math::cross(edgeNormal, poly.plane.normal));
        poly.edgePlanes[ii].origin = vertices[ii];
    }

    return poly;
}

Scene::TexturePixel Scene::getTexturePixel(const Scene::Material& mat, const math::Vec3f& pos) const
{
    if (mat.texture > -1)
    {
        auto uv = mat.positionToUV(pos);
        auto& data = textures[mat.texture];
        const Texture& tex = data.texture;
        const auto& fullbright = data.fullbright;
        const int x = normalize(uv.x, tex.getWidth());
        const int y = normalize(uv.y, tex.getHeight());
        Color c = tex.sample(x, y) + mat.color;
        Color::normalize(&c);
        const int idx = x + y * tex.getWidth();
        return {c, fullbright[idx]};
    }
    else
    {
        return {mat.color, false};
    }
}

Scene::TexturePixel Scene::getSkyPixel(const Material& mat, const Ray& ray, const Camera& camera, const math::Vec2i& screen) const
{
    auto skyDir = ray.dir;
    skyDir += camera.direction * 4096;
    skyDir.z *= 3;
    math::normalize(&skyDir);

    static const int SKY_SIZE = 1 << 7;
    static const int SKY_SPAN_SHIFT = 4; // slightly tweaked (was 5 in original code)

    int temp = screen.x >= screen.y ? screen.x : screen.y;

    math::Vec2f uv;
    uv.x = static_cast<int>((temp + 6*(SKY_SIZE/2-1)*skyDir.x) * 0x10000) >> SKY_SPAN_SHIFT;
    uv.y = static_cast<int>((temp + 6*(SKY_SIZE/2-1)*skyDir.y) * 0x10000) >> SKY_SPAN_SHIFT;

    const Texture& tex = textures[mat.texture].texture;
    const int x = normalize(uv.x, tex.getWidth() >> 1);
    const int y = normalize(uv.y, tex.getHeight());
    Color frontPlane = tex.sample(x, y);
    Color backPlane = tex.sample(x + (tex.getWidth() >> 1), y);
    if (!Color::isBlack(frontPlane))
    {
        backPlane = frontPlane;
    }
    Color::normalize(&backPlane);

    return {backPlane, true};
}
