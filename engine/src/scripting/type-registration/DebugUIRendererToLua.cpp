#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "subsystems/ScriptingSubsystem.h"
#include "subsystems/RenderSubsystem.h"
#include "Render/DebugRenderer.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterDebugTypes(sol::state &luaState)
{
    using namespace Blainn;

    // Register a DebugRenderer usertype (no constructor exposed to Lua)
    sol::table debugTable = luaState.create_table();

    debugTable.set_function("DrawLine",
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [](Vec3 &from, Vec3 &to, Vec4 &color)
        { RenderSubsystem::GetInstance().GetDebugRenderer().DrawLine({from, to, color}); });
    debugTable.set_function("DrawArrow",
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [](Vec3 &from, Vec3 &to, Vec4 &color, float size)
        { RenderSubsystem::GetInstance().GetDebugRenderer().DrawArrow(from, to, color, size); });
    debugTable.set_function("DrawTriangle",
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [](Vec3 &inV1, Vec3 &inV2, Vec3 &inV3, Vec4& inColor)
        { RenderSubsystem::GetInstance().GetDebugRenderer().DrawTriangle(inV1, inV2, inV3, inColor); });
    debugTable.set_function("DrawWireBox",
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [](Vec3 &min, Vec3 &max, Vec4 &inColor)
        { RenderSubsystem::GetInstance().GetDebugRenderer().DrawWireBox(min, max, inColor); });
    debugTable.set_function("DrawWireSphere",
        [](Vec3 &center, float radius, Vec4 &color)
        { RenderSubsystem::GetInstance().GetDebugRenderer().DrawWireSphere(center, radius, color); });
    debugTable.set_function("DrawCapsule",
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [](Mat4 &matrix, float halfHeightOfCylinder, float radius, Vec4 &color)
        {
            RenderSubsystem::GetInstance().GetDebugRenderer().DrawCapsule(
                {matrix, halfHeightOfCylinder, radius, color});
        });
    debugTable.set_function("DrawCylinder",
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        [](Mat4 &matrix, float halfHeight, float radius, Color color)
        { RenderSubsystem::GetInstance().GetDebugRenderer().DrawCylinder({matrix, halfHeight, radius, color}); });

    luaState["Debug"] = debugTable;
}

#endif
