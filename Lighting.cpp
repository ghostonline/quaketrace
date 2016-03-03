#include "Lighting.hpp"
#include "Scene.hpp"
#include "Util.hpp"
#include "Ray.hpp"
#include "Collision3D.hpp"
#include "Math.hpp"

const float Lighting::calcLightLevel(const math::Vec3f& origin, const math::Vec3f& hitNormal, const Scene& scene) const
{
    float lightLevel = 0.0f;
    for (int ii = util::lastIndex(directional); ii >= 0; --ii)
    {
        const Directional& light = directional[ii];
        Ray lightRay{ origin, -light.normal };
        if (!collision3d::raySceneCollision(lightRay, Scene::DIRECTIONAL_RAY_LENGTH, scene))
        {
            float factor = math::max(0.0f, math::dot(lightRay.dir, hitNormal));
            lightLevel += factor * light.intensity;
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
                static const float anglescale = 0.5f;
                float angle = (1.0f - anglescale) + anglescale * light.calcContribution(hitNormal, lightRay.dir);
                lightLevel += totalLightLevel * angle;
            }
        }
    }

    return math::clamp01(lightLevel);
}
