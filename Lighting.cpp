#include "Lighting.hpp"
#include "Scene.hpp"
#include "Util.hpp"
#include "Ray.hpp"
#include "Collision3D.hpp"
#include "Math.hpp"

static const float DIRECTIONAL_RAY_LENGTH = 1000.0f;

inline float applyAngleScale(float value)
{
    static const float anglescale = 0.5f;
    return (1.0f - anglescale) + anglescale * value;
}

const float Lighting::calcLightLevel(const math::Vec3f& origin, const math::Vec3f& hitNormal, const Scene& scene) const
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
        if (light.isShiningAtPoint(origin, hitNormal))
        {
            Ray lightRay{ origin, light.origin - origin };
            float rayLength = math::length(lightRay.dir);
            lightRay.dir /= rayLength;
            if (!collision3d::raySceneCollision(lightRay, rayLength, scene))
            {
                const float totalLightLevel = light.calcLightAtDistance(rayLength);
                const float factor = light.calcContribution(hitNormal, lightRay.dir);
                lightLevel += totalLightLevel * applyAngleScale(factor);
            }
        }
    }

    return math::clamp01(lightLevel);
}
