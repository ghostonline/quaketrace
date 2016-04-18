#pragma once

#include "Scene.hpp"
#include "Vec3.hpp"
#include "Lighting.hpp"

struct BspEntity;

struct BspLoader
{
    struct CameraDefinition
    {
        math::Vec3f origin;
        math::Vec3f direction;
    };

    static const Scene createSceneFromBsp(const void* data, int size);
    static const CameraDefinition parseIntermissionCamera(const BspEntity& entity);
    static const CameraDefinition parsePlayerStart(const BspEntity& entity);
    static const Lighting::Point parsePointLight(const BspEntity& entity);
    static const Lighting::Spot parseSpotLight(const BspEntity& entity, const math::Vec3f& targetOrigin);
    static void printBspAsObj(const void* data, int size);
};
