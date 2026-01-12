#pragma once

#include "UtilitySelector.h"

namespace Blainn
{
class UtilityBuilder
{
public:
    static std::unique_ptr<UtilitySelector> Build(sol::table luaTable);
};
}