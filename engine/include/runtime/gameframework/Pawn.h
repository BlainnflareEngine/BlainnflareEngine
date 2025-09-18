#pragma once

#include "gameframework/Actor.h"

namespace Blainn
{
    class Pawn : public Actor
    {
    public:
        Pawn() = default;
        
        virtual ~Pawn() override = default;
    };
}