#include "subsystems/ScriptingSubsystem.h"

#include <cassert>

using namespace Blainn;

bool ScriptingSubsystem::m_isInitialized = false;

void ScriptingSubsystem::Init() {
    m_isInitialized = true;
}

void ScriptingSubsystem::Destroy() {

}

