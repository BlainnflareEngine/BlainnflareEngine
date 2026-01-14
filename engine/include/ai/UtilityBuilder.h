#pragma once

#include "EASTL/vector.h"
#include "UtilitySelector.h"

namespace Blainn
{
class UtilityBuilder
{
public:
    static eastl::unique_ptr<UtilitySelector> Build(sol::table luaTable);
};
}