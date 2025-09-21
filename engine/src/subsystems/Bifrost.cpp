#include "subsystems/Bifrost.h"

#include <cassert>

using namespace Blainn;

bool Bifrost::m_isInitialized = false;

void Bifrost::Init() {
    m_isInitialized = true;
}

void Bifrost::Destroy() {

}

