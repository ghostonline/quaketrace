#pragma once

#include "Vec3.hpp"
#include "Color.hpp"
#include <vector>

struct Scene;

struct Lighting
{
    struct Point
    {
        Point(const math::Vec3f& origin, float strength, float sourceRadius)
        : origin(origin)
        , strength(strength)
        , attenuation(1.0f)
        , range(strength * 1.0f)
        , sourceRadius(sourceRadius)
        , color(1.0f, 1.0f, 1.0f)
        {}

        math::Vec3f origin;
        float strength;
        float attenuation;      // falloff factor (defaults to 1.0)
        float range;            // max distance the light will reach, based on strength and attenuation
        float sourceRadius;     // radius used for soft shadowing
        Color color;

        const bool isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& planeNormal) const;
        static const bool isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& planeNormal, const math::Vec3f& lightOrigin, float range);
        const float calcLightAtDistance(float dist) const;
        const float calcContribution(const math::Vec3f& planeNormal, const math::Vec3f& rayNormal) const;
        const std::vector<math::Vec3f> getRandomLightPoints(const math::Vec3f& castNormal, int count) const;
    };

    struct Directional
    {
        math::Vec3f normal;
        float intensity;

        const bool isShiningAtPoint(const math::Vec3f& planeNormal) const;
        const float calcContribution(const math::Vec3f& planeNormal, const math::Vec3f& rayNormal) const;
    };

    std::vector<Point> points;
    std::vector<Directional> directional;
    float ambient;

    Lighting() : ambient(0.0f) {}
    const float calcLightLevel(const math::Vec3f& origin, const math::Vec3f& hitNormal, const Scene& scene, int softShadowRays) const;
    void add(const Point& light) { points.push_back(light); }
    void add(const Directional& light) { directional.push_back(light); }
};

inline const bool Lighting::Point::isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& planeNormal) const
{
    return isShiningAtPoint(planeOrigin, planeNormal, origin, range);
}

inline const bool Lighting::Point::isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& planeNormal, const math::Vec3f& lightOrigin, float lightRange)
{
    const auto lightBeam = planeOrigin - lightOrigin;
    return math::length(lightBeam) < lightRange && math::dot(planeNormal, lightBeam) < 0;
}

inline const float Lighting::Point::calcLightAtDistance(float dist) const
{
    return (strength - dist * attenuation) / 255.0f;
}

inline const float Lighting::Point::calcContribution(const math::Vec3f& planeNormal, const math::Vec3f& rayNormal) const
{
    return math::max(0.0f, math::dot(rayNormal, planeNormal));
}

inline const bool Lighting::Directional::isShiningAtPoint(const math::Vec3f& planeNormal) const
{
    return math::dot(planeNormal, normal) < 0;
}

inline const float Lighting::Directional::calcContribution(const math::Vec3f& planeNormal, const math::Vec3f& rayNormal) const
{
    return math::max(0.0f, math::dot(planeNormal, rayNormal));
}
