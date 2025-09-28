#include "subsystems/PhysicsSubsystem.h"

#include <cassert>

using namespace Blainn;

bool PhysicsSubsystem::m_isInitialized = false;

void PhysicsSubsystem::Init() {
    m_isInitialized = true;
}

void PhysicsSubsystem::Destroy() {

}