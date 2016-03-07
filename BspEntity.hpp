#pragma once

#include <string>
#include <vector>
#include <array>
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
            KEY_WAIT,
            KEY_DELAY,
            KEY_COLOR,
            KEY_MODEL,
            NUM_KEYS,
        };

        static Property NULL_PROPERTY;

        std::string keyName;
        std::string value;

        Key key;
        math::Vec3f vec;
        float number;
        int integer;

        Property() : key(KEY_OTHER), vec{0.0f, 0.0f, 0.0f}, number(0.0f), integer(0) {}
        static const Property parse(const util::ArrayView<char>& serialized);
    };


    BspEntity();
    const bool hasProperty(Property::Key key) const;
    const Property& getProperty(Property::Key key) const;

    static const BspEntity parse(const util::ArrayView<char>& serialized);
    static std::vector<BspEntity> parseList(const util::ArrayView<char>& serialized);

    std::array<int, Property::NUM_KEYS> keyIndices;
    Type type;
    std::vector<Property> properties;
};

inline BspEntity::BspEntity() : type(TYPE_OTHER)
{
    for (int ii = 0; ii < Property::NUM_KEYS; ++ii)
    {
        keyIndices[ii] = -1;
    }
}

inline const bool BspEntity::hasProperty(Property::Key key) const
{
    return key != Property::KEY_OTHER && keyIndices[key] > -1;
}

inline const BspEntity::Property& BspEntity::getProperty(Property::Key key) const
{
    return hasProperty(key) ? properties[keyIndices[key]] : Property::NULL_PROPERTY;
}
