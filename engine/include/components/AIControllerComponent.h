#pragma once

#include "EASTL/string.h"
#include "EASTL/unordered_map.h"
#include "EASTL/variant.h"
#include "ai/AIController.h"
#include "scripting/LuaScript.h"

namespace Blainn
{

struct AIControllerComponent
{
    std::string scriptPath;
    AIController aiController;
    eastl::unique_ptr<LuaScript> aiScript;

    float MovementSpeed = 2;
    float StoppingDistance = 0.5;
    float GroundOffset = 0.5;
    bool FaceMovementDirection = true;
    float RotationSpeed = 0.5f;
};

} // namespace Blainn