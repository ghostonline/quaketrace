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
        enum Key
        {
            KEY_OTHER = -1,
            KEY_CLASSNAME,
            KEY_ORIGIN,
            KEY_MANGLE,
            KEY_ANGLE,
            KEY_LIGHT,
            NUM_KEYS,
        };

        Key key;
        std::string keyName;
        std::string value;

        static const Property parse(const util::ArrayView<char>& serialized);
    };


    BspEntity() : type(TYPE_OTHER) {}
    static const BspEntity parse(const util::ArrayView<char>& serialized);
    static std::vector<BspEntity> parseList(const util::ArrayView<char>& serialized);

    Type type;
    std::vector<Property> properties;
};
