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

    template<typename T>
    inline bool isInFrontOfPlane(const Scene::Plane& plane, const T& geometry);

    template<>
    inline bool isInFrontOfPlane<math::Vec3f>(const Scene::Plane& plane, const math::Vec3f& point)
    {
        auto diff = point - plane.origin;
        return math::dot(diff, plane.normal) >= 0;
    }

    template<>
    inline bool isInFrontOfPlane<Scene::Sphere>(const Scene::Plane& plane, const Scene::Sphere& sphere)
    {
        auto nearest = sphere.origin + plane.normal * sphere.radius;
        return isInFrontOfPlane(plane, sphere.origin) || isInFrontOfPlane(plane, nearest);
    }

    template<>
    inline bool isInFrontOfPlane<Scene::Triangle>(const Scene::Plane& plane, const Scene::Triangle& triangle)
    {
        return isInFrontOfPlane(plane, triangle.a) || isInFrontOfPlane(plane, triangle.b) || isInFrontOfPlane(plane, triangle.c);
    }

    template<>
    inline bool isInFrontOfPlane<Scene::ConvexPolygon>(const Scene::Plane& plane, const Scene::ConvexPolygon& poly)
    {
        for (int ii = util::lastIndex(poly.vertices); ii >= 0; --ii)
        {
            if (isInFrontOfPlane(plane, poly.vertices[ii]))
            {
                return true;
            }
        }
        return false;
    }

    template<>
    inline bool isInFrontOfPlane<Scene::Plane>(const Scene::Plane& plane, const Scene::Plane& poly)
    {
        return true;
    }

    template<typename T>
    const std::vector<T> testCulling(const std::vector<T>& geometryList, const Scene::Plane frustum[4])
    {
        std::vector<T> optimized;
        optimized.reserve(geometryList.size());
        for (int ii = util::lastIndex(geometryList); ii >= 0; --ii)
        {
            auto& geometry = geometryList[ii];
            for (int jj = 0; jj < 4; ++jj)
            {
                if (!isInFrontOfPlane(frustum[jj], geometry))
                {
                    goto reject;
                    break;
                }
            }
            optimized.push_back(geometry);

        reject:
            ;
        }
        return optimized;
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

Scene Scene::createShadowScene(const Scene& scene)
{
    Scene shadowScene;
    shadowScene.lighting = scene.lighting;
    shadowScene.cameras = scene.cameras;
    shadowScene.spheres = scene.spheres;
    shadowScene.planes = scene.planes;
    shadowScene.triangles = scene.triangles;
    shadowScene.polygons.reserve(scene.polygons.size());
    for (int ii = util::lastIndex(scene.polygons); ii >= 0; --ii)
    {
        const auto& poly = scene.polygons[ii];
        if (poly.flags[Scene::ConvexPolygon::FLAG_SHADOWCAST])
        {
            shadowScene.polygons.push_back(poly);
        }
    }
    return shadowScene;
}

Scene Scene::cullGeometry(const Scene& scene, const Camera& camera)
{
    Scene optimized;
    const math::Vec3f up = math::normalized(camera.direction + camera.up * camera.halfViewAngles.y);
    const math::Vec3f down = math::normalized(camera.direction - camera.up * camera.halfViewAngles.y);
    const math::Vec3f left = math::normalized(camera.direction - camera.right * camera.halfViewAngles.x);
    const math::Vec3f right = math::normalized(camera.direction + camera.right * camera.halfViewAngles.x);
    const math::Vec3f topNormal = math::cross(up, camera.right);
    const math::Vec3f bottomNormal = math::cross(down, -camera.right);
    const math::Vec3f leftNormal = math::cross(left, camera.up);
    const math::Vec3f rightNormal = math::cross(right, -camera.up);
    Plane cullPlanes[4] = {
        {camera.origin, topNormal, Color()},
        {camera.origin, bottomNormal, Color()},
        {camera.origin, leftNormal, Color()},
        {camera.origin, rightNormal, Color()},
    };
    optimized.spheres = testCulling<Sphere>(scene.spheres, cullPlanes);
    optimized.planes = testCulling<Plane>(scene.planes, cullPlanes);
    optimized.triangles = testCulling<Triangle>(scene.triangles, cullPlanes);
    optimized.polygons = testCulling<ConvexPolygon>(scene.polygons, cullPlanes);
    optimized.textures = scene.textures;
    return optimized;
}

const Scene::ConvexPolygon Scene::ConvexPolygon::create(const std::vector<math::Vec3f>& vertices, const math::Vec3f& normal, const Material& material)
{
    ASSERT(vertices.size() > 2);

    ConvexPolygon poly;
    poly.material = material;
    poly.plane.normal = normal;
    poly.plane.origin = vertices[0];
    poly.vertices = vertices;

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
