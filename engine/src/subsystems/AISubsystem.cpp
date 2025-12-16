#include <pch.h>
#include "subsystems/AISubsystem.h"
#include "AISubsystem.h"

using namespace Blainn;

AISubsystem &AISubsystem::GetInstance()
{
    static AISubsystem instance;
    return instance;
}

void AISubsystem::Init()
{
    BF_INFO("AISubsystem Init");
}

void AISubsystem::Destroy()
{
    BF_INFO("AISubsystem Destroy");
}