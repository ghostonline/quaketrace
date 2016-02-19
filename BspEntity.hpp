#pragma once

#include <string>
#include <vector>
#include "ArrayView.hpp"
#include "Math.hpp"

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

        std::string keyName;
        std::string value;

        Key key;
        math::Vec3f vec;
        float number;
        int integer;

        Property() : key(KEY_OTHER), vec{0.0f, 0.0f, 0.0f}, number(0.0f), integer(0) {}
        static const Property parse(const util::ArrayView<char>& serialized);
    };


    BspEntity() : type(TYPE_OTHER) {}
    static const BspEntity parse(const util::ArrayView<char>& serialized);
    static std::vector<BspEntity> parseList(const util::ArrayView<char>& serialized);

    Type type;
    std::vector<Property> properties;
};
