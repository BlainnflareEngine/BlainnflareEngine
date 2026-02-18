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
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        sol::meta_function::addition, [](const Vec3 &a, const Vec3 &b) { return a + b; },
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        sol::meta_function::subtraction, [](const Vec3 &a, const Vec3 &b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            [](const Vec3 &a, const Vec3 &b) { return a * b; },
            [](const Vec3 &v, float s) { return v * s; },
            [](float s, const Vec3 &v) { return s * v; }
            ),
        sol::meta_function::division, sol::overload(
            // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            [](const Vec3 &a, const Vec3 &b) { return a / b; },
            [](const Vec3 &v, float s) { return v / s; },
            [](float s, const Vec3 &v) { return s / v; }
            ),
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        sol::meta_function::equal_to, [](const Vec3 &a, const Vec3 &b) { return a == b; },
        sol::meta_function::unary_minus, [](const Vec3 &a) { return -a; }
    );

    Vec3Type["x"] = &Vec3::x;
    Vec3Type["y"] = &Vec3::y;
    Vec3Type["z"] = &Vec3::z;

    // Member functions
    Vec3Type.set_function("InBounds",        &Vec3::InBounds);
    Vec3Type.set_function("Length",          &Vec3::Length);
    Vec3Type.set_function("LengthSquared",   &Vec3::LengthSquared);
    Vec3Type.set_function("Normalize",       static_cast<void (Vec3::*)()>(&Vec3::Normalize));
    Vec3Type.set_function("Dot",             static_cast<float (Vec3::*)(const Vec3 &) const>(&Vec3::Dot));
    Vec3Type.set_function("Cross",           static_cast<Vec3 (Vec3::*)(const Vec3 &) const>(&Vec3::Cross));
    Vec3Type.set_function("Clamp",           static_cast<void (Vec3::*)(const Vec3&, const Vec3&)>(&Vec3::Clamp));

    // Static functions
    Vec3Type.set_function("Distance",        static_cast<float (*)(const Vec3&, const Vec3&)>(&Vec3::Distance));
    Vec3Type.set_function("DistanceSquared", static_cast<float (*)(const Vec3&, const Vec3&)>(&Vec3::DistanceSquared));
    Vec3Type.set_function("Min",             static_cast<Vec3 (*)(const Vec3&, const Vec3&)>(&Vec3::Min));
    Vec3Type.set_function("Max",             static_cast<Vec3 (*)(const Vec3&, const Vec3&)>(&Vec3::Max));
    Vec3Type.set_function("Lerp",            static_cast<Vec3 (*)(const Vec3&, const Vec3&, float)>(&Vec3::Lerp));
    Vec3Type.set_function("SmoothStep",      static_cast<Vec3 (*)(const Vec3&, const Vec3&, float)>(&Vec3::SmoothStep));
    Vec3Type.set_function("Barycentric",     static_cast<Vec3 (*)(const Vec3&, const Vec3&, const Vec3&, float, float)>(&Vec3::Barycentric));
    Vec3Type.set_function("CatmullRom",      static_cast<Vec3 (*)(const Vec3&, const Vec3&, const Vec3&, const Vec3&, float)>(&Vec3::CatmullRom));
    Vec3Type.set_function("Hermite",         static_cast<Vec3 (*)(const Vec3&, const Vec3&, const Vec3&, const Vec3&, float)>(&Vec3::Hermite));
    Vec3Type.set_function("Reflect",         static_cast<Vec3 (*)(const Vec3&, const Vec3&)>(&Vec3::Reflect));
    Vec3Type.set_function("Refract",         static_cast<Vec3 (*)(const Vec3&, const Vec3&, float)>(&Vec3::Refract));
    Vec3Type.set_function("TransformQuat",   static_cast<Vec3 (*)(const Vec3&, const Quat&)>(&Vec3::Transform));
    Vec3Type.set_function("TransformMat4",   static_cast<Vec3 (*)(const Vec3&, const Mat4&)>(&Vec3::Transform));
    Vec3Type.set_function("TransformNormal", static_cast<Vec3 (*)(const Vec3&, const Mat4&)>(&Vec3::TransformNormal));

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
