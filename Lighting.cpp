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

template<typename T>
float calcLightingForLightType(const std::vector<T>& lights, const Scene& scene, const math::Vec3f& origin, const math::Vec3f& hitNormal, int softShadowRays)
{
    float lightLevel = 0.0f;
    for (int ii = util::lastIndex(lights); ii >= 0; --ii)
    {
        const auto& light = lights[ii];
        auto castRay = math::normalized(light.origin - origin);
        auto lightRays = light.getRandomLightPoints(castRay, softShadowRays);
        lightRays.push_back(light.origin);
        const float rayContribution = 1.0f / lightRays.size();
        for (int ii = util::lastIndex(lightRays); ii >= 0; --ii )
        {
            auto lightOrigin = lightRays[ii];
            if (light.isShiningAtPoint(origin, hitNormal, lightOrigin))
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
    return lightLevel;
}

const float Lighting::calcLightLevel(const math::Vec3f& origin, const math::Vec3f& hitNormal, const Scene& scene, int softShadowRays, int occlusionRays) const
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

    lightLevel += calcLightingForLightType<Point>(points, scene, origin, hitNormal, softShadowRays);
    lightLevel += calcLightingForLightType<Spot>(spots, scene, origin, hitNormal, softShadowRays);
    lightLevel = math::clamp01(lightLevel);

    if (lightLevel > 0.0f && occlusionRays > 0)
    {
        static const float AMBIENT_OCCLUSION_STRENGTH = 16;
        std::vector<math::Vec3f> occlusion = getPointsOnUnitSphere(occlusionRays);
        int occlusionHits = 0;
        for (int ii = util::lastIndex(occlusion); ii >= 0; --ii)
        {
            auto& dir = occlusion[ii];
            dir *= math::dot(hitNormal, dir);
            math::normalize(&dir);
            Ray ray = {origin, dir};
            occlusionHits += collision3d::raySceneCollision(ray, AMBIENT_OCCLUSION_STRENGTH, scene) & 1;
        }
        float occlusionFactor = 1.0f - occlusionHits / static_cast<float>(occlusionRays);
        lightLevel *= occlusionFactor;
    }

    return lightLevel;
}

const std::vector<math::Vec3f> Lighting::getPointsOnUnitSphere(int count) const
{
    std::vector<math::Vec3f> directions(count);
    for (int ii = count - 1; ii >= 0; --ii)
    {
        float azimuth = util::Random::rand(0, math::PI2);
        float zenith = std::acos(2.0f - util::Random::randFloat() - 1);
        directions[ii].x = std::cos(azimuth) * std::sin(zenith);
        directions[ii].y = std::sin(azimuth) * std::sin(zenith);
        directions[ii].z = std::cos(zenith);
    }
    return directions;
}

const std::vector<math::Vec3f> Lighting::getPointsOnDisk(int count, const math::Vec3f& origin, const math::Vec3f& normal, float radius)
{
    std::vector<math::Vec3f> points(count);
    static const math::Vec3f kindaUp = {0.0f, 0.0f, 1.0f};
    auto right = math::cross(normal, kindaUp);
    if (math::length2(right) < math::APPROXIMATE_ZERO)
    {
        static const math::Vec3f kindaUp = {0.0f, 1.0f, 0.0f};
        right = math::cross(normal, kindaUp);
    }
    const auto up = math::cross(normal, right);
    float radiusSqrt = std::sqrt(radius);
    for (int ii = count - 1; ii >= 0; --ii)
    {
        auto point = origin;
        float theta = util::Random::rand(0, math::PI2);
        point += up * std::sin(theta) * radiusSqrt;
        point += right * std::cos(theta) * radiusSqrt;
        points[ii] = point;
    }
    return points;
}
