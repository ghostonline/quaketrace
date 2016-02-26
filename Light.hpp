#pragma once

#include "Vec3.hpp"
#include "Color.hpp"

struct PointLight
{
    // Lifted from quake light tool
    enum Formula {
        FORMULA_LINEAR,     /* Linear (x) (DEFAULT) */
        FORMULA_INVERSE,    /* Inverse (1/x), scaled by 1/128 */
        FORMULA_INVERSE2,   /* Inverse square (1/(x^2)), scaled by 1/(128^2) */
        FORMULA_INFINITE,   /* No attenuation, same brightness at any distance */
        FORMULA_LOCALMIN,   /* No attenuation, non-additive minlight effect within
                             line of sight of the light source. */
        FORMULA_INVERSE2A,	/* Inverse square, with distance adjusted to avoid
                             exponentially bright values near the source.
                             (1/(x+128)^2), scaled by 1/(128^2) */
        NUM_FORMULA
    };

    PointLight(const math::Vec3f& origin, float strength)
    : origin(origin)
    , strength(strength)
    , attenuation(1.0f)
    , range(strength)
    , formula(FORMULA_LINEAR)
    , color(1.0f, 1.0f, 1.0f)
    {}

    math::Vec3f origin;
    float strength;
    float attenuation;      // falloff factor (defaults to 1.0)
    float range;            // max distance the light will reach, based on the formula and strength
    Formula formula;
    Color color;

    const bool isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& planeNormal) const;
    const float calcContribution(float dist) const;
};

inline const bool PointLight::isShiningAtPoint(const math::Vec3f& planeOrigin, const math::Vec3f& planeNormal) const
{
    const auto lightBeam = planeOrigin - origin;
    return math::length(lightBeam) < range && math::dot(planeNormal, lightBeam) < 0;
}

inline const float PointLight::calcContribution(float dist) const
{
    return (strength - dist) / 255.0f;
}
