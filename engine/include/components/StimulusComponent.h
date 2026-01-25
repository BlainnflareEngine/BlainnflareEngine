#pragma once

#include "EASTL/string.h"
#include "aliases.h"
#include "PerceptionComponent.h"

namespace Blainn
{

struct StimulusComponent
{
    bool enableSight = true;
    bool enableSound = true;
    bool enableTouch = true;
    bool enableDamage = true;
    
    float sightRadius = 0.0f;
    float soundRadius = 0.0f;
    
    eastl::string tag = "Untagged";
    
    bool enabled = true;
    
    uint32_t GetStimulusMask() const
    {
        uint32_t mask = 0;
        if (enableSight) mask |= (1u << static_cast<uint8_t>(StimulusType::Sight));
        if (enableSound) mask |= (1u << static_cast<uint8_t>(StimulusType::Sound));
        if (enableTouch) mask |= (1u << static_cast<uint8_t>(StimulusType::Touch));
        if (enableDamage) mask |= (1u << static_cast<uint8_t>(StimulusType::Damage));
        return mask;
    }
    
    bool HasStimulus(StimulusType type) const
    {
        switch (type)
        {
            case StimulusType::Sight: return enableSight;
            case StimulusType::Sound: return enableSound;
            case StimulusType::Touch: return enableTouch;
            case StimulusType::Damage: return enableDamage;
            default: return false;
        }
    }
};

} // namespace Blainn