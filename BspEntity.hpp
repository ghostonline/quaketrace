#pragma once

#include <string>
#include <vector>
#include "ArrayView.hpp"

struct BspEntity
{
    enum Type
    {
        TYPE_OTHER = -1,
        TYPE_WORLD_SPAWN,
        TYPE_PLAYER_START,
        TYPE_INTERMISSION_CAMERA,
        TYPE_LIGHT,
        NUM_TYPES,
    };

    struct Property
    {
        std::string key;
        std::string value;

        static const Property parse(const util::ArrayView<char>& serialized);
    };


    static const BspEntity parse(const util::ArrayView<char>& serialized);
    static std::vector<BspEntity> parseList(const util::ArrayView<char>& serialized);

    Type type;
    std::vector<Property> properties;
};
