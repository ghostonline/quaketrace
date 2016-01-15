#include "BspLoader.hpp"

const Scene BspLoader::createSceneFromBsp(const void* data, int size)
{
    Scene scene;
    Scene::initDefault(&scene);
    return scene;
}
