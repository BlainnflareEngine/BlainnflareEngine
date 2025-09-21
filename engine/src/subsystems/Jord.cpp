#include "subsystems/Jord.h"

#include <cassert>

using namespace Blainn;

bool Jord::m_isInitialized = false;

void Jord::Init() {
    m_isInitialized = true;
}

void Jord::Destroy() {

}