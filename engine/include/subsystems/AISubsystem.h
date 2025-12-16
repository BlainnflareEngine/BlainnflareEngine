#pragma once
#include "helpers.h"
#include "ai/BTBuilder.h"

namespace Blainn
{
class AISubsystem
{
public:
    NO_COPY_NO_MOVE(AISubsystem);

    static AISubsystem &GetInstance();

    void Init();
    void Destroy();

private:
    AISubsystem() = default;

private:
};
} // namespace Blainn