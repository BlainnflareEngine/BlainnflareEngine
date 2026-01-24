//
// Created by WhoLeb on 16-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "pch.h"
#include "aliases.h"

#include "scripting/TypeRegistration.h"

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterVector3Type(sol::state &luaState)
{
    sol::usertype<Vec3> Vec3Type = luaState.new_usertype<Vec3>(
        "Vec3",
        sol::constructors<
            Vec3(),
            Vec3(float),
            Vec3(float, float, float)
        >(),
        sol::meta_function::addition, [](const Vec3 &a, const Vec3 &b) { return a + b; },
        sol::meta_function::subtraction, [](const Vec3 &a, const Vec3 &b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const Vec3 &a, const Vec3 &b) { return a * b; },
            [](const Vec3 &v, float s) { return v * s; },
            [](float s, const Vec3 &v) { return s * v; }
            ),
        sol::meta_function::division, sol::overload(
            [](const Vec3 &a, const Vec3 &b) { return a / b; },
            [](const Vec3 &v, float s) { return v / s; },
            [](float s, const Vec3 &v) { return s / v; }
            ),
        sol::meta_function::equal_to, [](const Vec3 &a, const Vec3 &b) { return a == b; },
        sol::meta_function::unary_minus, [](const Vec3 &a) { return -a; }
    );

    Vec3Type["x"] = &Vec3::x;
    Vec3Type["y"] = &Vec3::y;
    Vec3Type["z"] = &Vec3::z;

    // Member functions
    Vec3Type["InBounds"] = &Vec3::InBounds;
    Vec3Type["Length"] = static_cast<float (Vec3::*)() const>(&Vec3::Length);
    Vec3Type["LengthSquared"] = static_cast<float (Vec3::*)() const>(&Vec3::LengthSquared);
    Vec3Type["Normalize"] = static_cast<void (Vec3::*)()>(&Vec3::Normalize);
    Vec3Type["Dot"] = static_cast<float (Vec3::*)(const Vec3 &) const>(&Vec3::Dot);
    Vec3Type["Cross"] = static_cast<Vec3 (Vec3::*)(const Vec3 &) const>(&Vec3::Cross);
    Vec3Type["Clamp"] = static_cast<void (Vec3::*)(const Vec3&, const Vec3&)>(&Vec3::Clamp);

    // Static functions
    Vec3Type["Distance"] = static_cast<float (*)(const Vec3&, const Vec3&)>(&Vec3::Distance);
    Vec3Type["DistanceSquared"] = static_cast<float (*)(const Vec3&, const Vec3&)>(&Vec3::DistanceSquared);
    Vec3Type["Min"] = static_cast<Vec3 (*)(const Vec3&, const Vec3&)>(&Vec3::Min);
    Vec3Type["Max"] = static_cast<Vec3 (*)(const Vec3&, const Vec3&)>(&Vec3::Max);
    Vec3Type["Lerp"] = static_cast<Vec3 (*)(const Vec3&, const Vec3&, float)>(&Vec3::Lerp);
    Vec3Type["SmoothStep"] = static_cast<Vec3 (*)(const Vec3&, const Vec3&, float)>(&Vec3::SmoothStep);
    Vec3Type["Barycentric"] = static_cast<Vec3 (*)(const Vec3&, const Vec3&, const Vec3&, float, float)>(&Vec3::Barycentric);
    Vec3Type["CatmullRom"] = static_cast<Vec3 (*)(const Vec3&, const Vec3&, const Vec3&, const Vec3&, float)>(&Vec3::CatmullRom);
    Vec3Type["Hermite"] = static_cast<Vec3 (*)(const Vec3&, const Vec3&, const Vec3&, const Vec3&, float)>(&Vec3::Hermite);
    Vec3Type["Reflect"] = static_cast<Vec3 (*)(const Vec3&, const Vec3&)>(&Vec3::Reflect);
    Vec3Type["Refract"] = static_cast<Vec3 (*)(const Vec3&, const Vec3&, float)>(&Vec3::Refract);
    Vec3Type["TransformQuat"] = static_cast<Vec3 (*)(const Vec3&, const Quat&)>(&Vec3::Transform);
    Vec3Type["TransformMat4"] = static_cast<Vec3 (*)(const Vec3&, const Mat4&)>(&Vec3::Transform);
    Vec3Type["TransformNormal"] = static_cast<Vec3 (*)(const Vec3&, const Mat4&)>(&Vec3::TransformNormal);

    // Constants
    luaState["Vec3Zero"] = Vec3::Zero;
    luaState["Vec3One"] = Vec3::One;
    luaState["Vec3UnitX"] = Vec3::UnitX;
    luaState["Vec3UnitY"] = Vec3::UnitY;
    luaState["Vec3UnitZ"] = Vec3::UnitZ;
    luaState["Vec3Up"] = Vec3::Up;
    luaState["Vec3Down"] = Vec3::Down;
    luaState["Vec3Right"] = Vec3::Right;
    luaState["Vec3Left"] = Vec3::Left;
    luaState["Vec3Forward"] = Vec3::Forward;
    luaState["Vec3Backward"] = Vec3::Backward;
}

#endif