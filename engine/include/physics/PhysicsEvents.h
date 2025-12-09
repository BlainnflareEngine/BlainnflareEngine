#pragma once

#include "aliases.h"

namespace Blainn
{

enum class PhysicsEventType
{
    CollisionStarted,
    CollisionEnded
};

struct PhysicsEvent
{
    PhysicsEventType eventType;
    uuid entity1;
    uuid entity2;
};

struct PhysicsEventPolicy
{
    static PhysicsEventType getEvent(const eastl::shared_ptr<PhysicsEvent> &physicsEvent)
    {
        return physicsEvent->eventType;
    }
};

}