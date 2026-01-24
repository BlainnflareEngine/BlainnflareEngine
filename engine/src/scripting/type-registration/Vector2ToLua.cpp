//
// Created by WhoLeb on 16-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "pch.h"
#include "aliases.h"

#include "scripting/TypeRegistration.h"

#ifdef BLAINN_REGISTER_LUA_TYPES
namespace Blainn
{
void RegisterVector2Type(sol::state &luaState)
{
    sol::usertype<Vec2> Vec2Type = luaState.new_usertype<Vec2>(
        "Vec2",
        sol::constructors<
            Vec2(),
            Vec2(float, float)
        >(),
        sol::meta_function::addition, [](const Vec2 &a, const Vec2 &b) { return a + b; },
        sol::meta_function::subtraction, [](const Vec2 &a, const Vec2 &b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const Vec2 &a, const Vec2 &b) { return a * b; },
            [](const Vec2 &v, float s) { return v * s; },
            [](float s, const Vec2 &v) { return s * v; }
            ),
        sol::meta_function::division, sol::overload(
            [](const Vec2 &a, const Vec2 &b) { return a / b; },
            [](const Vec2 &v, float s) { return v / s; },
            [](float s, const Vec2 &v) { return s / v; }
            ),        sol::meta_function::equal_to, [](const Vec2 &a, const Vec2 &b) { return a == b; },
        sol::meta_function::unary_minus, [](const Vec2 &a) { return -a; }
    );

    Vec2Type["x"] = &Vec2::x;
    Vec2Type["y"] = &Vec2::y;

    // Member functions
    Vec2Type["Length"] = static_cast<float (Vec2::*)() const>(&Vec2::Length);
    Vec2Type["LengthSquared"] = static_cast<float (Vec2::*)() const>(&Vec2::LengthSquared);
    Vec2Type["Normalize"] = static_cast<void (Vec2::*)()>(&Vec2::Normalize);
    Vec2Type["Dot"] = static_cast<float (Vec2::*)(const Vec2 &) const>(&Vec2::Dot);
    Vec2Type["Cross"] = static_cast<Vec2 (Vec2::*)(const Vec2 &) const>(&Vec2::Cross);
    Vec2Type["Clamp"] = static_cast<void (Vec2::*)(const Vec2&, const Vec2&)>(&Vec2::Clamp);

    // Static functions
    Vec2Type["Distance"] = static_cast<float (*)(const Vec2&, const Vec2&)>(&Vec2::Distance);
    Vec2Type["DistanceSquared"] = static_cast<float (*)(const Vec2&, const Vec2&)>(&Vec2::DistanceSquared);
    Vec2Type["Min"] = static_cast<Vec2 (*)(const Vec2&, const Vec2&)>(&Vec2::Min);
    Vec2Type["Max"] = static_cast<Vec2 (*)(const Vec2&, const Vec2&)>(&Vec2::Max);
    Vec2Type["Lerp"] = static_cast<Vec2 (*)(const Vec2&, const Vec2&, float)>(&Vec2::Lerp);
    Vec2Type["SmoothStep"] = static_cast<Vec2 (*)(const Vec2&, const Vec2&, float)>(&Vec2::SmoothStep);
    Vec2Type["Barycentric"] = static_cast<Vec2 (*)(const Vec2&, const Vec2&, const Vec2&, float, float)>(&Vec2::Barycentric);
    Vec2Type["CatmullRom"] = static_cast<Vec2 (*)(const Vec2&, const Vec2&, const Vec2&, const Vec2&, float)>(&Vec2::CatmullRom);
    Vec2Type["Hermite"] = static_cast<Vec2 (*)(const Vec2&, const Vec2&, const Vec2&, const Vec2&, float)>(&Vec2::Hermite);
    Vec2Type["Reflect"] = static_cast<Vec2 (*)(const Vec2&, const Vec2&)>(&Vec2::Reflect);
    Vec2Type["Refract"] = static_cast<Vec2 (*)(const Vec2&, const Vec2&, float)>(&Vec2::Refract);
    Vec2Type["TransformQuat"] = static_cast<Vec2 (*)(const Vec2&, const Quat&)>(&Vec2::Transform);
    Vec2Type["TransformMat4"] = static_cast<Vec2 (*)(const Vec2&, const Mat4&)>(&Vec2::Transform);
    Vec2Type["TransformNormal"] = static_cast<Vec2 (*)(const Vec2&, const Mat4&)>(&Vec2::TransformNormal);

    // Constants
    luaState["Vec2Zero"] = Vec2::Zero;
    luaState["Vec2One"] = Vec2::One;
    luaState["Vec2UnitX"] = Vec2::UnitX;
    luaState["Vec2UnitY"] = Vec2::UnitY;
}
}
#endif