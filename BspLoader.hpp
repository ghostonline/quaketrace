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
    static const Lighting::Point parseLight(const BspEntity& entity);
    static void printBspAsObj(const void* data, int size);
};
