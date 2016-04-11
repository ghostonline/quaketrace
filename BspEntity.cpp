#include "BspEntity.hpp"
#include <cstdio>
#include "Util.hpp"
#include "StringTool.hpp"

static const std::string ENTITY_TYPE_KEY = "classname";
struct
{
    const char* classname;
    BspEntity::Type type;
} ENTITY_TYPE_DATA[] = {
    { "worldspawn", BspEntity::TYPE_WORLD_SPAWN, },
    { "info_player_start", BspEntity::TYPE_PLAYER_START, },
    { "info_intermission", BspEntity::TYPE_INTERMISSION_CAMERA, },
    { "light", BspEntity::TYPE_LIGHT, },
    { "light_fluoro", BspEntity::TYPE_LIGHT, },
    { "light_fluorospark", BspEntity::TYPE_LIGHT, },
    { "light_globe", BspEntity::TYPE_LIGHT, },
    { "light_flame_large_yellow", BspEntity::TYPE_LIGHT, },
    { "light_flame_small_yellow", BspEntity::TYPE_LIGHT, },
    { "light_flame_small_white", BspEntity::TYPE_LIGHT, },
    { "light_torch_small_walltorch", BspEntity::TYPE_LIGHT, },
};

enum KeyType
{
    TYPE_UNKNOWN = -1,
    TYPE_VEC,
    TYPE_NUMBER,
    TYPE_INTEGER,
    TYPE_BRUSH,
};

struct
{
    const char* keyName;
    KeyType type;
} ENTITY_PROPERTY_KEY_DATA[] = {
    { "classname",	TYPE_UNKNOWN,},
    { "origin",		TYPE_VEC,    },
    { "mangle",		TYPE_VEC,    },
    { "angle",		TYPE_NUMBER, },
    { "light",		TYPE_NUMBER, },
    { "wait",		TYPE_NUMBER, },
    { "delay",		TYPE_INTEGER,},
    { "color",		TYPE_VEC,    },
    { "model",		TYPE_BRUSH,  },
};


STATIC_ASSERT_ARRAY_SIZE(ENTITY_PROPERTY_KEY_DATA, BspEntity::Property::NUM_KEYS);

static const std::vector<std::string> extractKeyNames()
{
    std::vector<std::string> names(BspEntity::Property::NUM_KEYS);
    for (int ii = 0; ii < BspEntity::Property::NUM_KEYS; ++ii)
    {
        names[ii] = ENTITY_PROPERTY_KEY_DATA[ii].keyName;
    }
    return names;
}

static const std::vector<std::string> extractClassNames()
{
    std::vector<std::string> names(UTIL_ARRAY_SIZE(ENTITY_TYPE_DATA));
    for (int ii = 0; ii < UTIL_ARRAY_SIZE(ENTITY_TYPE_DATA); ++ii)
    {
        names[ii] = ENTITY_TYPE_DATA[ii].classname;
    }
    return names;
}

static const std::vector<std::string> KEY_NAMES = extractKeyNames();
static const std::vector<std::string> CLASS_NAMES = extractClassNames();

BspEntity::Property BspEntity::Property::NULL_PROPERTY;

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

            if (prop.key != Property::KEY_OTHER)
            {
                entity.keyIndices[prop.key] = util::lastIndex(entity.properties);
            }
        }
    }

    if (entity.hasProperty(Property::KEY_CLASSNAME))
    {
        const Property& prop = entity.getProperty(Property::KEY_CLASSNAME);
        const util::ArrayView<std::string> view = { CLASS_NAMES.data(), static_cast<int>(CLASS_NAMES.size()) };
        int idx = util::findItemInArray(view, prop.value);
        entity.type = ENTITY_TYPE_DATA[idx].type;
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

    const util::ArrayView<std::string> view = { KEY_NAMES.data(), static_cast<int>(KEY_NAMES.size()) };
    int idx = util::findItemInArray(view, property.keyName);
    property.key = static_cast<Key>(idx);

    if (property.key != KEY_OTHER)
    {
        const auto& keyData = ENTITY_PROPERTY_KEY_DATA[property.key];
        switch(keyData.type)
        {
            case TYPE_VEC:
                util::StringTool::parseVec3f(property.value.c_str(), &property.vec);
                break;
            case TYPE_NUMBER:
                util::StringTool::parseFloat(property.value.c_str(), &property.number);
                break;
            case TYPE_INTEGER:
                util::StringTool::parseInteger(property.value.c_str(), &property.integer);
                break;
            case TYPE_BRUSH:
                ASSERT(property.value[0] == '*');
                util::StringTool::parseInteger(property.value.c_str(), &property.integer);
                break;
            default:
                break;
        }
    }

    ASSERT(!property.keyName.empty() && !property.value.empty());
    return property;
}
