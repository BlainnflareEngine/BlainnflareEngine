#pragma once

#include "components/ActorComponent.h"

namespace Blainn
{    
    class SceneComponent : public ActorComponent
    {
    public:
        SceneComponent();
        
        virtual ~SceneComponent() = default;
        
    public:
        virtual void OnInitialize() override;
        virtual void BeginPlay() override;
        virtual void OnDestroy() override;
    };
}