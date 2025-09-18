#pragma once

#include "core/Object.h"

namespace Blainn 
{
    class Actor : public Object
    {
    public:
        Actor() = default;
        
        virtual ~Actor() override = default;
    };    
}