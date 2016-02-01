#include "Collision3D.hpp"
#include "Ray.hpp"
#include "Util.hpp"
#include "Assert.hpp"
#include <limits>

inline bool rayPlaneIntersection(const Ray& ray, const math::Vec3f& planeOrigin, const math::Vec3f& planeNormal, float* t)
{
    ASSERT(t);
    float denom = math::dot(ray.dir, planeNormal);
    if (denom > -math::APPROXIMATE_ZERO) { return false; }
    auto relativeOrigin = planeOrigin - ray.origin;
    *t = math::dot(relativeOrigin, planeNormal) / denom;
    return true;
}

int collision3d::raycastSpheres(const Ray& ray, float maxDist, const std::vector<Scene::Sphere>& spheres, Hit* hitResult)
{
    float minDist = maxDist;
    int minIndex = -1;
    for (int ii = util::lastIndex(spheres); ii >= 0; --ii)
    {
        const Scene::Sphere& sphere = spheres[ii];
        auto relativeOrigin = sphere.origin - ray.origin;

        // Figure out when ray passes the sphere center
        float dot = math::dot(relativeOrigin, ray.dir);
        // When light starts beyond sphere center, just ignore it
        // NOTE: this is not correct when ray starts within radius!
        if (dot < 0) { continue; }
        auto projection = ray.dir * dot;

        // If the ray passes the sphere center at a point beyond the radius, the sphere will not be hit
        const float distance2 = math::distance2(projection, relativeOrigin);
        const float radius2 = math::squared(sphere.radius);
        if (distance2 > radius2) { continue; }

        float penetration = std::sqrt(radius2 - distance2);
        float dist = dot - penetration;
        if (minDist > dist)
        {
            minIndex = ii;
            minDist = dist;
        }
    }

    if (minIndex > -1 && hitResult)
    {
        hitResult->pos = ray.dir * minDist + ray.origin;
        hitResult->normal = math::normalized(hitResult->pos - spheres[minIndex].origin);
        hitResult->t = minDist;
    }

    return minIndex;
}

int collision3d::raycastPlanes(const Ray& ray, float maxDist, const std::vector<Scene::Plane>& planes, Hit* hitResult)
{
    float minDist = maxDist;
    int minIndex = -1;
    math::Vec3f minIntersection;
    for (int ii = util::lastIndex(planes); ii >= 0; --ii)
    {
        const Scene::Plane& plane = planes[ii];

        float denom = math::dot(ray.dir, plane.normal);
        // Only render plane if point moves toward front of the plane
        if (denom > math::APPROXIMATE_ZERO) { continue; }

        // Determine intersection time
        auto relativeOrigin = plane.origin - ray.origin;
        float t = math::dot(relativeOrigin, plane.normal) / denom;

        // Calculate intersection point relative to the ray origin
        auto intersection = ray.dir * t;
        float dist = math::length(intersection);
        if (minDist > dist)
        {
            minDist = dist;
            minIndex = ii;
            minIntersection = intersection;
        }
    }

    if (minIndex > -1 && hitResult)
    {
        hitResult->pos = ray.origin + minIntersection;
        hitResult->normal = planes[minIndex].normal;
        hitResult->t = minDist;
    }

    return minIndex;
}

int collision3d::raycastTriangles(const Ray& ray, float maxDist, const std::vector<Scene::Triangle>& triangles, Hit* hitResult)
{

    float minDist = maxDist;
    int minIndex = -1;
    math::Vec3f minNormal;
    for (int ii = util::lastIndex(triangles); ii >= 0; --ii)
    {
        const Scene::Triangle& triangle = triangles[ii];

        // TODO: Precalculate this?
        math::Vec3f edgeAB = triangle.b - triangle.a;
        math::Vec3f edgeAC = triangle.c - triangle.a;
        math::Vec3f triangleNormal = math::normalized(math::cross(edgeAB, edgeAC));

        // Find intersection point with plane
        float dist = 0.0f;
        bool intersects = rayPlaneIntersection(ray, triangle.a, triangleNormal, &dist);
        if (!intersects || minDist < dist) { continue; }

        auto relativeIntersection = (ray.dir * dist) + (ray.origin - triangle.a);

        // Check ray intersects in triangle boundaries (source: http://geomalgorithms.com/a04-_planes.html#Barycentric-Coordinate-Compute)
        // NOTE: slightly more complicated than 3 extra dot products to determine which side of each edge plane the point lies
        const math::Vec3f& u = edgeAB;
        const math::Vec3f& v = edgeAC;
        const math::Vec3f& w = relativeIntersection;
        float uv = math::dot(u, v);
        float wv = math::dot(w, v);
        float vv = math::dot(v, v);
        float wu = math::dot(w, u);
        float uu = math::dot(u, u);
        float denom2 = math::squared(uv) - uu * vv;
        float sI = (uv*wv - vv*wu) / denom2;
        float tI = (uv*wu - uu*wv) / denom2;
        if (sI < 0.0f || tI < 0.0f || (sI + tI) > 1.0f) { continue; }

        minDist = dist;
        minIndex = ii;
        minNormal = triangleNormal;
    }

    if (minIndex > -1 && hitResult)
    {
        hitResult->pos = ray.dir * minDist + ray.origin;
        hitResult->normal = minNormal;
        hitResult->t = minDist;
    }

    return minIndex;
}

int collision3d::raycastConvexPolygons(const Ray& ray, float maxDist, const std::vector<Scene::ConvexPolygon>& polygons, Hit* hitResult)
{
    float minDist = maxDist;
    int minIndex = -1;
    math::Vec3f minNormal;
    for (int ii = util::lastIndex(polygons); ii >= 0; --ii)
    {
        const Scene::ConvexPolygon& poly = polygons[ii];
        ASSERT(poly.vertices.size() > 2);

        // TODO: Precalculate this
        // Create edge normals
        std::vector<math::Vec3f> normals(poly.vertices.size());
        for (int ii = util::lastIndex(normals) - 1; ii >= 0; --ii)
        {
            normals[ii] = poly.vertices[ii + 1] - poly.vertices[ii];
            math::normalize(&normals[ii]);
        }
        // Special case: last normal wraps around
        normals.back() = poly.vertices.front() - poly.vertices.back();
        math::normalize(&normals.back());

        // Create polygon normal (this is not safe when both edges are the same)
        const math::Vec3f polyNormal = math::normalized(math::cross(normals[0], normals[1]));

        // Create edge/poly planes
        struct Plane { math::Vec3f normal; math::Vec3f origin; };
        std::vector<Plane> edgePolyPlanes(normals.size());
        for (int ii = util::lastIndex(normals); ii >= 0; --ii)
        {
            const math::Vec3f& edgeNormal = normals[ii];
            edgePolyPlanes[ii].normal = math::normalized(math::cross(polyNormal, edgeNormal));
            edgePolyPlanes[ii].origin = poly.vertices[ii];
        }

        // Actual collision algorithm

        // Perform plane intersection
        float dist = 0.0f;
        bool intersects = rayPlaneIntersection(ray, poly.vertices[0], polyNormal, &dist);
        if (!intersects || dist > minDist) { continue; }

        math::Vec3f intersection = ray.dir * dist + ray.origin;

        // Detect whether intersection point lies in front of each edge plane
        bool insidePolygon = true;
        for (int ii = util::lastIndex(edgePolyPlanes); ii >= 0; --ii)
        {
            const auto& plane = edgePolyPlanes[ii];
            auto relative = intersection - plane.origin;
            if (math::dot(relative, plane.normal) < 0)
            {
                // behind plane
                insidePolygon = false;
                break;
            }
        }

        if (!insidePolygon) { continue; }

        minDist = dist;
        minIndex = ii;
        minNormal = polyNormal;
    }

    if (minIndex > -1 && hitResult)
    {
        hitResult->pos = ray.origin + ray.dir * minDist;
        hitResult->normal = minNormal;
        hitResult->t = minDist;
    }

    return minIndex;
}
