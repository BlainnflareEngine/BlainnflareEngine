#include "subsystems/Bifrost.h"

#include <cassert>

using namespace Blainn;

Bifrost::Bifrost() {
    
}

Bifrost& Bifrost::GetInstance() {
    assert(m_isInitialized && "Bifrost subsystem not initialized");
    static Bifrost instance;
    return instance;
}

void Bifrost::Init() {
    m_isInitialized = true;
}

void Bifrost::Destroy() {

}

