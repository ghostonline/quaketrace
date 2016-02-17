#include "BspEntity.hpp"
#include <cstdio>
#include "Util.hpp"

static const std::string ENTITY_TYPE_KEY = "classname";
static const std::string ENTITY_TYPE_NAMES[] = {
    "worldspawn",
    "info_player_start",
    "info_intermission",
    "light",
};

STATIC_ASSERT_ARRAY_SIZE(ENTITY_TYPE_NAMES, BspEntity::NUM_TYPES);

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

            if (prop.key == ENTITY_TYPE_KEY)
            {
                int idx = util::findItemInArray({ ENTITY_TYPE_NAMES, BspEntity::NUM_TYPES }, prop.value);
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
                property.key = std::string(serialized.array + start, ii - start);
                key = false;
            }
            else
            {
                property.value = std::string(serialized.array + start, ii - start);
            }
        }
    }

    ASSERT(!property.key.empty() && !property.value.empty());
    return property;
}
