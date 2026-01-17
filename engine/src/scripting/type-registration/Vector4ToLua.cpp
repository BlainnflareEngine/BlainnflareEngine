//
// Created by WhoLeb on 16-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "pch.h"
#include "aliases.h"

#include "scripting/TypeRegistration.h"

#ifdef BLAINN_REGISTER_LUA_TYPES
using namespace Blainn;

void Blainn::RegisterVector4Type(sol::state &luaState)
{
    sol::usertype<Vec4> Vec4Type = luaState.new_usertype<Vec4>(
        "Vec4", sol::constructors<Vec4(), Vec4(float, float, float, float)>(), sol::meta_function::subtraction,
        [](const Vec4 &a, const Vec4 &b) { return a - b; }, sol::meta_function::addition,
        [](const Vec4 &a, const Vec4 &b) { return a + b; }, sol::meta_function::unary_minus, [](const Vec4 &a)
        { return -a; }, sol::meta_function::multiplication, [](const Vec4 &a, float s) { return a * s; },
        sol::meta_function::division, [](const Vec4 &a, float s) { return a / s; }, sol::meta_function::equal_to,
        [](const Vec4 &a, const Vec4 &b) { return a == b; });

    Vec4Type["x"] = &Vec4::x;
    Vec4Type["y"] = &Vec4::y;
    Vec4Type["z"] = &Vec4::z;
    Vec4Type["w"] = &Vec4::w;

    Vec4Type[sol::meta_function::call] = [](float x, float y, float z, float w) { return Vec4(x, y, z, w); };

    Vec4Type["Length"] = static_cast<float (Vec4::*)() const>(&Vec4::Length);
    Vec4Type["LengthSquared"] = static_cast<float (Vec4::*)() const>(&Vec4::LengthSquared);
    Vec4Type["Normalize"] = static_cast<void (Vec4::*)()>(&Vec4::Normalize);
    Vec4Type["Dot"] = static_cast<float (Vec4::*)(const Vec4 &) const>(&Vec4::Dot);
    Vec4Type["Distance"] = static_cast<float (*)(const Vec4 &, const Vec4 &)>(&Vec4::Distance);
    Vec4Type["Lerp"] = static_cast<Vec4 (*)(const Vec4 &, const Vec4 &, float)>(&Vec4::Lerp);
}

#endif