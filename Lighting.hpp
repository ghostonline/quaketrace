#pragma once

#include "Vec3.hpp"
#include "Color.hpp"
#include <vector>

struct Scene;

struct Lighting
{
    struct PositionedLight
    {
        PositionedLight(const math::Vec3f& origin, float strength, float sourceRadius)
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

        static const bool isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& lightOrigin, float range);
        const float calcLightAtDistance(float dist) const;
    };

    struct Point : public PositionedLight
    {
        Point(const math::Vec3f& origin, float strength, float sourceRadius)
        : PositionedLight(origin, strength, sourceRadius)
        {}

        const bool isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& lightOrigin) const;
        const bool isShiningAtPoint(const math::Vec3f& planeOrigin) const;
        const float calcContribution(const math::Vec3f& planeNormal, const math::Vec3f& rayNormal) const;
        const std::vector<math::Vec3f> getRandomLightPoints(const math::Vec3f& castNormal, int count) const;
    };

    struct Directional
    {
        math::Vec3f normal;
        float intensity;

        const float calcContribution(const math::Vec3f& planeNormal, const math::Vec3f& rayNormal) const;
    };

    struct Spot : public PositionedLight
    {
        Spot(const math::Vec3f& origin, float strength, float sourceRadius, const math::Vec3f& normal, float angleFalloff)
        : PositionedLight(origin, strength, sourceRadius)
        , normal(normal)
        , angleFalloff(angleFalloff)
        {}

        math::Vec3f normal;
        float angleFalloff;

        const bool isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& lightOrigin) const;
        const bool isShiningAtPoint(const math::Vec3f& planeOrigin) const;
        const float calcContribution(const math::Vec3f& planeNormal, const math::Vec3f& rayNormal) const;
        const std::vector<math::Vec3f> getRandomLightPoints(const math::Vec3f& castNormal, int count) const;
    };

    std::vector<Point> points;
    std::vector<Directional> directional;
    std::vector<Spot> spots;
    float ambient;

    Lighting() : ambient(0.0f) {}
    const float calcLightLevel(const math::Vec3f& origin, const math::Vec3f& hitNormal, const Scene& scene, int softShadowRays, int occlusionRays, int occlusionRayStrength) const;
    const std::vector<math::Vec3f> getPointsOnUnitSphere(int count) const;
    static const std::vector<math::Vec3f> getPointsOnDisk(int count, const math::Vec3f& origin, const math::Vec3f& normal, float radius);

    void add(const Point& light) { points.push_back(light); }
    void add(const Directional& light) { directional.push_back(light); }
    void add(const Spot& light) { spots.push_back(light); }
};

inline const bool Lighting::PositionedLight::isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& lightOrigin, float lightRange)
{
    const auto lightBeam = planeOrigin - lightOrigin;
    return math::length(lightBeam) < lightRange;
}

inline const float Lighting::PositionedLight::calcLightAtDistance(float dist) const
{
    return (strength - dist * attenuation) / 255.0f;
}

inline const float Lighting::Point::calcContribution(const math::Vec3f& planeNormal, const math::Vec3f& rayNormal) const
{
    return math::max(0.0f, math::dot(rayNormal, planeNormal));
}

inline const bool Lighting::Point::isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& lightOrigin) const
{
    return PositionedLight::isShiningAtPoint(planeOrigin, lightOrigin, range);
}

inline const bool Lighting::Point::isShiningAtPoint(const math::Vec3f& planeOrigin) const
{
    return PositionedLight::isShiningAtPoint(planeOrigin, origin, range);
}

inline const std::vector<math::Vec3f> Lighting::Point::getRandomLightPoints(const math::Vec3f& castNormal, int count) const
{
    return Lighting::getPointsOnDisk(count, origin, castNormal, sourceRadius);
}

inline const float Lighting::Directional::calcContribution(const math::Vec3f& planeNormal, const math::Vec3f& rayNormal) const
{
    return math::max(0.0f, math::dot(planeNormal, rayNormal));
}

inline const bool Lighting::Spot::isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& lightOrigin) const
{
    if (!PositionedLight::isShiningAtPoint(planeOrigin, lightOrigin, range))
    {
        return false;
    }

    const auto rayDir = math::normalized(lightOrigin - planeOrigin);
    return math::dot(normal, rayDir) <= angleFalloff;
}

inline const bool Lighting::Spot::isShiningAtPoint(const math::Vec3f& planeOrigin) const
{
    return isShiningAtPoint(planeOrigin, origin);
}

inline const float Lighting::Spot::calcContribution(const math::Vec3f& planeNormal, const math::Vec3f& rayNormal) const
{
    return math::max(0.0f, math::dot(planeNormal, rayNormal));
}

inline const std::vector<math::Vec3f> Lighting::Spot::getRandomLightPoints(const math::Vec3f& castNormal, int count) const
{
    return Lighting::getPointsOnDisk(count, origin, castNormal, sourceRadius);
}
