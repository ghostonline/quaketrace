#include "Lighting.hpp"
#include "Scene.hpp"
#include "Util.hpp"
#include "Ray.hpp"
#include "Collision3D.hpp"
#include "Math.hpp"
#include "Random.hpp"

static const float DIRECTIONAL_RAY_LENGTH = 1000.0f;

inline float applyAngleScale(float value)
{
    static const float anglescale = 0.5f;
    return (1.0f - anglescale) + anglescale * value;
}

const float Lighting::calcLightLevel(const math::Vec3f& origin, const math::Vec3f& hitNormal, const Scene& scene, int softShadowRays) const
{
    float lightLevel = ambient;
    for (int ii = util::lastIndex(directional); ii >= 0; --ii)
    {
        const Directional& light = directional[ii];
        if (light.isShiningAtPoint(hitNormal))
        {
            Ray lightRay{ origin, -light.normal };
            if (!collision3d::raySceneCollision(lightRay, DIRECTIONAL_RAY_LENGTH, scene))
            {
                const float factor = light.calcContribution(hitNormal, lightRay.dir);
                lightLevel += applyAngleScale(factor) * light.intensity;
            }
        }
    }

    for (int ii = util::lastIndex(points); ii >= 0; --ii)
    {
        const auto& light = points[ii];
        auto castRay = math::normalized(light.origin - origin);
        auto lightRays = light.getRandomLightPoints(castRay, softShadowRays);
        lightRays.push_back(light.origin);
        const float rayContribution = 1.0f / lightRays.size();
        for (int ii = util::lastIndex(lightRays); ii >= 0; --ii )
        {
            auto lightOrigin = lightRays[ii];
            if (Point::isShiningAtPoint(origin, hitNormal, lightOrigin, light.range))
            {
                Ray lightRay{ origin, lightOrigin - origin };
                float rayLength = math::length(lightRay.dir);
                lightRay.dir /= rayLength;
                if (!collision3d::raySceneCollision(lightRay, rayLength, scene))
                {
                    const float totalLightLevel = light.calcLightAtDistance(rayLength);
                    const float factor = light.calcContribution(hitNormal, lightRay.dir);
                    lightLevel += totalLightLevel * applyAngleScale(factor) * rayContribution;
                }
            }
        }
    }

    return math::clamp01(lightLevel);
}

const std::vector<math::Vec3f> Lighting::Point::getRandomLightPoints(const math::Vec3f& castNormal, int count) const
{
    std::vector<math::Vec3f> points(count);
    static const math::Vec3f kindaUp = {0.0f, 0.0f, 1.0f};
    auto right = math::cross(castNormal, kindaUp);
    if (math::length2(right) < math::APPROXIMATE_ZERO)
    {
        static const math::Vec3f kindaUp = {0.0f, 1.0f, 0.0f};
        right = math::cross(castNormal, kindaUp);
    }
    const auto up = math::cross(castNormal, right);
    float radiusSqrt = std::sqrt(sourceRadius);
    for (int ii = count - 1; ii >= 0; --ii)
    {
        auto point = origin;
        float theta = Random::rand(0, math::PI2);
        point += up * std::sin(theta) * radiusSqrt;
        point += right * std::cos(theta) * radiusSqrt;
        points[ii] = point;
    }
    return points;
}
