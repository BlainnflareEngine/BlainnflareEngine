#include "subsystems/Jord.h"

#include <cassert>

using namespace Blainn;

Jord::Jord() {
    
}

Jord& Jord::GetInstance() {
    assert(m_isInitialized && "Jord subsystem not initialized");
    static Jord instance;
    return instance;
}

void Jord::Init() {
    m_isInitialized = true;
}

void Jord::Destroy() {

}