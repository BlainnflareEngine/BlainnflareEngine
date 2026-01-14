#pragma once

#include "EASTL/shared_ptr.h"
#include "aliases.h"
#include "components/PerceptionComponent.h"

namespace Blainn
{

enum class PerceptionEventType
{
    StimulusPerceived,
    StimulusForgotten,
    EnemySpotted, // priority tag
    EnemyLost,
};

struct PerceptionEvent
{
    PerceptionEventType type;
    uuid observerEntity;
    uuid stimulusEntity;
    StimulusType stimulusType;
    Vec3 location;
    
    virtual ~PerceptionEvent() = default;
};

using PerceptionEventPointer = eastl::shared_ptr<PerceptionEvent>;

struct PerceptionEventPolicy
{
    static PerceptionEventType getEvent(const PerceptionEventPointer& e)
    {
        return e->type;
    }
};

} // namespace Blainn