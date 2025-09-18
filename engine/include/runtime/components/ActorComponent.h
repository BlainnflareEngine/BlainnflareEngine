#pragma once

#include "core/Object.h"

class Actor;

namespace Blainn
{
    class ActorComponent : public Object
    {
    public:
        ActorComponent() = default;
        
        virtual ~ActorComponent() override = default;
        
    public:
        virtual void OnInitialize();
        virtual void BeginPlay();
        virtual void OnDestroy();
        
        Actor* GetOwner() const;

        template<typename T>
        T* GetOwner() const
        {
            // temporary
            return nullptr;
        }
    };
}