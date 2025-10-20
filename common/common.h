#pragma once

#include <entt/entt.hpp>

#include "aliases.h"

namespace Blainn 
{
    inline static entt::registry& GetRegistry(){static entt::registry registry; return registry;};
}