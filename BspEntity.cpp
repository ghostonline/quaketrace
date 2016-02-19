#include "BspEntity.hpp"
#include <cstdio>
#include "Util.hpp"
#include "StringTool.hpp"

static const std::string ENTITY_TYPE_KEY = "classname";
static const std::string ENTITY_TYPE_NAMES[] = {
    "worldspawn",
    "info_player_start",
    "info_intermission",
    "light",
};

STATIC_ASSERT_ARRAY_SIZE(ENTITY_TYPE_NAMES, BspEntity::NUM_TYPES);

static const std::string ENTITY_PROPERTY_KEY_NAMES[] = {
    "classname",
    "origin",
    "mangle",
    "angle",
    "light",
};

STATIC_ASSERT_ARRAY_SIZE(ENTITY_PROPERTY_KEY_NAMES, BspEntity::Property::NUM_KEYS);

std::vector<BspEntity> BspEntity::parseList(const util::ArrayView<char>& serialized)
{
    std::vector<BspEntity> entities;
    for (int ii = 0; ii < serialized.size; ++ii)
    {
        if (serialized[ii] == '{')
        {
            int entityStart = ii;
            int indent = 1;
            while (++ii < serialized.size && indent)
            {
                char tok = serialized[ii];
                if (tok == '}') { --indent; }
                else if (tok == '{') { ++indent; }
            }
            const BspEntity entity = parse(serialized.slice(entityStart, ii));
            entities.push_back(entity);
        }
    }
    return entities;
}

const BspEntity BspEntity::parse(const util::ArrayView<char>& serialized)
{
    BspEntity entity;
    for (int ii = 0; ii < serialized.size; ++ii)
    {
        if (serialized[ii] == '"')
        {
            int propStart = ii;
            while(ii < serialized.size && serialized[ii] != '\n') { ++ii; }
            Property prop = Property::parse(serialized.slice(propStart, ii));
            entity.properties.push_back(prop);

            if (prop.key == Property::KEY_CLASSNAME)
            {
                int idx = util::findItemInArray({ ENTITY_TYPE_NAMES, NUM_TYPES }, prop.value);
                entity.type = static_cast<Type>(idx);
            }
        }
    }
    return entity;
}

const BspEntity::Property BspEntity::Property::parse(const util::ArrayView<char>& serialized)
{
    Property property;
    bool key = true;
    for (int ii = 0; ii < serialized.size; ++ii)
    {
        if (serialized[ii] == '"')
        {
            ++ii;
            int start = ii;
            while (ii < serialized.size && serialized[ii] != '"')
            {
                ++ii;
            }
            if (key)
            {
                property.keyName = std::string(serialized.array + start, ii - start);
                key = false;
            }
            else
            {
                property.value = std::string(serialized.array + start, ii - start);
            }
        }
    }

    int idx = util::findItemInArray({ENTITY_PROPERTY_KEY_NAMES, NUM_KEYS}, property.keyName);
    property.key = static_cast<Key>(idx);

    switch(property.key)
    {
        default:
            break;
        case KEY_ORIGIN:
        case KEY_MANGLE:
            property.vec = util::StringTool::parseVec3f(property.value.c_str());
            break;
        case KEY_ANGLE:
        case KEY_LIGHT:
            property.number = util::StringTool::parseFloat(property.value.c_str());
            break;
    }
    ASSERT(!property.keyName.empty() && !property.value.empty());
    return property;
}
