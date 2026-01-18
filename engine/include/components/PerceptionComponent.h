#pragma once

#include "EASTL/vector.h"
#include "EASTL/string.h"
#include "aliases.h"

namespace Blainn
{

enum class StimulusType : uint8_t
{
    Sight = 0,
    Sound = 1,
    Touch = 2,
    Damage = 3,
    Custom = 255
};

struct PerceivedStimulus
{
    uuid sourceEntity = {};
    StimulusType type;
    Vec3 location{0.0f};
    float age = 0.0f; // в секундах
    float strength = 1.0f; // 0 - 1
    eastl::string tag;
    bool successfullySensed = true;
};

struct PerceptionComponent
{
    bool enableSight = true;
    float sightRange = 1500.0f;
    float sightFOV = 90.0f;
    float sightForgetTime = 5.0f;
    float sightLOSCheckInterval = 0.1f;
    bool sightRequireLOS = true;
    
    bool enableSound = false;
    float soundRange = 2000.0f;
    float soundForgetTime = 3.0f;
    float soundMinStrength = 0.1f;
    
    bool enableTouch = false;
    float touchForgetTime = 1.0f;
    
    bool enableDamage = false;
    float damageForgetTime = 2.0f;
    
    eastl::vector<eastl::string> ignoreTags;
    eastl::vector<eastl::string> priorityTags;
    
    float updateInterval = 0.0f;
    float maxUpdateDistance = 5000.0f;
    
    bool enabled = true;
    
    eastl::vector<PerceivedStimulus> perceivedStimuli;
    float timeSinceLastUpdate = 0.0f;
    float cachedDistanceToCamera = 0.0f;
    
    bool ShouldIgnoreTag(const eastl::string& tag) const
    {
        for (const auto& ignoreTag : ignoreTags)
        {
            if (tag == ignoreTag)
                return true;
        }
        return false;
    }
    
    bool IsPriorityTag(const eastl::string& tag) const
    {
        for (const auto& priorityTag : priorityTags)
        {
            if (tag == priorityTag)
                return true;
        }
        return false;
    }
};

} // namespace Blainn