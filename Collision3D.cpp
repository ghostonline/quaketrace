#include "Collision3D.hpp"
#include "Ray.hpp"
#include "Util.hpp"
#include <limits>

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
    math::Vec3f minIntersection;
    math::Vec3f minNormal;
    for (int ii = util::lastIndex(triangles); ii >= 0; --ii)
    {
        const Scene::Triangle& triangle = triangles[ii];

        // TODO: Precalculate this?
        math::Vec3f edgeAB = triangle.b - triangle.a;
        math::Vec3f edgeAC = triangle.c - triangle.a;
        math::Vec3f triangleNormal = math::normalized(math::cross(edgeAB, edgeAC));

        // Find intersection point with plane
        float denom = math::dot(ray.dir, triangleNormal);
        if (denom > -math::APPROXIMATE_ZERO) { continue; }
        auto relativeOrigin = triangle.a - ray.origin;
        float t = math::dot(relativeOrigin, triangleNormal) / denom;
        auto intersection = ray.dir * t;

        float dist = math::length(intersection);
        if (minDist < dist) { continue; }

        // Small optimization
#if 0
        auto relativeIntersection = intersection + camera.origin - triangle.a;
#else
        auto relativeIntersection = intersection - relativeOrigin;
#endif

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
        minIntersection = intersection;
        minNormal = triangleNormal;
    }

    if (minIndex > -1 && hitResult)
    {
        hitResult->pos = minIntersection + ray.origin;
        hitResult->normal = minNormal;
        hitResult->t = minDist;
    }

    return minIndex;
}
