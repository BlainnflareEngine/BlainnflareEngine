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
        "Vec3", sol::constructors<Vec3(), Vec3(float, float, float)>(), sol::meta_function::subtraction,
        [](const Vec3 &a, const Vec3 &b) { return a - b; }, sol::meta_function::addition,
        [](const Vec3 &a, const Vec3 &b) { return a + b; }, sol::meta_function::unary_minus, [](const Vec3 &a)
        { return -a; }, sol::meta_function::multiplication, [](const Vec3 &a, float s) { return a * s; },
        sol::meta_function::division, [](const Vec3 &a, float s) { return a / s; }, sol::meta_function::equal_to,
        [](const Vec3 &a, const Vec3 &b) { return a == b; });

    Vec3Type["x"] = &Vec3::x;
    Vec3Type["y"] = &Vec3::y;
    Vec3Type["z"] = &Vec3::z;

    Vec3Type[sol::meta_function::call] = [](float x, float y, float z) { return Vec3(x, y, z); };

    Vec3Type["Length"] = static_cast<float (Vec3::*)() const>(&Vec3::Length);
    Vec3Type["LengthSquared"] = static_cast<float (Vec3::*)() const>(&Vec3::LengthSquared);
    Vec3Type["Normalize"] = static_cast<void (Vec3::*)()>(&Vec3::Normalize);
    Vec3Type["Dot"] = static_cast<float (Vec3::*)(const Vec3 &) const>(&Vec3::Dot);
    Vec3Type["Cross"] = static_cast<Vec3 (Vec3::*)(const Vec3 &) const>(&Vec3::Cross);
    Vec3Type["Distance"] = static_cast<float (*)(const Vec3 &, const Vec3 &)>(&Vec3::Distance);
    Vec3Type["Clamp"] = static_cast<void (Vec3::*)(const Vec3 &, const Vec3 &)>(&Vec3::Clamp);
    Vec3Type["Lerp"] = static_cast<Vec3 (*)(const Vec3 &, const Vec3 &, float)>(&Vec3::Lerp);
}

#endif