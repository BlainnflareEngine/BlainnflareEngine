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
            ),
        sol::meta_function::equal_to, [](const Vec2 &a, const Vec2 &b) { return a == b; },
        sol::meta_function::unary_minus, [](const Vec2 &a) { return -a; }
    );

    Vec2Type["x"] = &Vec2::x;
    Vec2Type["y"] = &Vec2::y;

    // Member functions
    Vec2Type.set_function("Length",          &Vec2::Length);
    Vec2Type.set_function("LengthSquared",   &Vec2::LengthSquared);
    Vec2Type.set_function("Normalize",       static_cast<void (Vec2::*)()>(&Vec2::Normalize));
    Vec2Type.set_function("Dot",             static_cast<float (Vec2::*)(const Vec2 &) const>(&Vec2::Dot));
    Vec2Type.set_function("Cross",           static_cast<Vec2 (Vec2::*)(const Vec2 &) const>(&Vec2::Cross));
    Vec2Type.set_function("Clamp",           static_cast<void (Vec2::*)(const Vec2&, const Vec2&)>(&Vec2::Clamp));

    // Static functions
    Vec2Type.set_function("Distance",        static_cast<float (*)(const Vec2&, const Vec2&)>(&Vec2::Distance));
    Vec2Type.set_function("DistanceSquared", static_cast<float (*)(const Vec2&, const Vec2&)>(&Vec2::DistanceSquared));
    Vec2Type.set_function("Min",             static_cast<Vec2 (*)(const Vec2&, const Vec2&)>(&Vec2::Min));
    Vec2Type.set_function("Max",             static_cast<Vec2 (*)(const Vec2&, const Vec2&)>(&Vec2::Max));
    Vec2Type.set_function("Lerp",            static_cast<Vec2 (*)(const Vec2&, const Vec2&, float)>(&Vec2::Lerp));
    Vec2Type.set_function("SmoothStep",      static_cast<Vec2 (*)(const Vec2&, const Vec2&, float)>(&Vec2::SmoothStep));
    Vec2Type.set_function("Barycentric",     static_cast<Vec2 (*)(const Vec2&, const Vec2&, const Vec2&, float, float)>(&Vec2::Barycentric));
    Vec2Type.set_function("CatmullRom",      static_cast<Vec2 (*)(const Vec2&, const Vec2&, const Vec2&, const Vec2&, float)>(&Vec2::CatmullRom));
    Vec2Type.set_function("Hermite",         static_cast<Vec2 (*)(const Vec2&, const Vec2&, const Vec2&, const Vec2&, float)>(&Vec2::Hermite));
    Vec2Type.set_function("Reflect",         static_cast<Vec2 (*)(const Vec2&, const Vec2&)>(&Vec2::Reflect));
    Vec2Type.set_function("Refract",         static_cast<Vec2 (*)(const Vec2&, const Vec2&, float)>(&Vec2::Refract));
    Vec2Type.set_function("TransformQuat",   static_cast<Vec2 (*)(const Vec2&, const Quat&)>(&Vec2::Transform));
    Vec2Type.set_function("TransformMat4",   static_cast<Vec2 (*)(const Vec2&, const Mat4&)>(&Vec2::Transform));
    Vec2Type.set_function("TransformNormal", static_cast<Vec2 (*)(const Vec2&, const Mat4&)>(&Vec2::TransformNormal));

    // Constants
    luaState["Vec2Zero"] = Vec2::Zero;
    luaState["Vec2One"] = Vec2::One;
    luaState["Vec2UnitX"] = Vec2::UnitX;
    luaState["Vec2UnitY"] = Vec2::UnitY;
}
}
#endif
