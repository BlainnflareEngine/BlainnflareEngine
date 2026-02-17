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
        "Vec4",
        // Constructors
        sol::constructors<
            Vec4(),
            Vec4(float),
            Vec4(float, float, float, float)
        >(),
        // Operators
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        sol::meta_function::addition, [](const Vec4& a, const Vec4& b) { return a + b; },
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        sol::meta_function::subtraction, [](const Vec4& a, const Vec4& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            [](const Vec4 &a, const Vec4 &b) { return a * b; },
            [](const Vec4 &v, float s) { return v * s; },
            [](float s, const Vec4 &v) { return s * v; }
            ),
        sol::meta_function::division, sol::overload(
            // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            [](const Vec4 &a, const Vec4 &b) { return a / b; },
            [](const Vec4 &v, float s) { return v / s; },
            [](float s, const Vec4 &v) { return s / v; }
            ),
        sol::meta_function::unary_minus, [](const Vec4& a) { return -a; },
        // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        sol::meta_function::equal_to, [](const Vec4& a, const Vec4& b) { return a == b; }
    );

    // Members
    Vec4Type["x"] = &Vec4::x;
    Vec4Type["y"] = &Vec4::y;
    Vec4Type["z"] = &Vec4::z;
    Vec4Type["w"] = &Vec4::w;

    // Call operator for convenience Vec4(x,y,z,w)
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    Vec4Type[sol::meta_function::call] = [](float x, float y, float z, float w)
    { return Vec4(x, y, z, w); };

    // Instance methods
    Vec4Type.set_function("Length",        &Vec4::Length);
    Vec4Type.set_function("LengthSquared", &Vec4::LengthSquared);
    Vec4Type.set_function("Normalize",     static_cast<void (Vec4::*)()>(&Vec4::Normalize));
    Vec4Type.set_function("Dot",           static_cast<float (Vec4::*)(const Vec4&) const>(&Vec4::Dot));
    Vec4Type.set_function("Cross",         sol::overload(
        static_cast<void (Vec4::*)(const Vec4&, const Vec4&, Vec4&) const>(&Vec4::Cross),
        static_cast<Vec4 (Vec4::*)(const Vec4&, const Vec4&) const>(&Vec4::Cross)
    ));
    Vec4Type.set_function("InBounds",      &Vec4::InBounds);
    Vec4Type.set_function("Clamp",         sol::overload(
        static_cast<void (Vec4::*)(const Vec4&, const Vec4&)>(&Vec4::Clamp),
        static_cast<void (Vec4::*)(const Vec4&, const Vec4&, Vec4&) const>(&Vec4::Clamp)
    ));

    // Static methods
    Vec4Type.set_function("Distance",        static_cast<float (*)(const Vec4&, const Vec4&)>(&Vec4::Distance));
    Vec4Type.set_function("DistanceSquared", static_cast<float (*)(const Vec4&, const Vec4&)>(&Vec4::DistanceSquared));

    Vec4Type.set_function("Min",             sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, Vec4&)>(&Vec4::Min),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&)>(&Vec4::Min)
    ));
    Vec4Type.set_function("Max",             sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, Vec4&)>(&Vec4::Max),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&)>(&Vec4::Max)
    ));
    Vec4Type.set_function("Lerp",            sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, float, Vec4&)>(&Vec4::Lerp),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, float)>(&Vec4::Lerp)
    ));
    Vec4Type.set_function("SmoothStep",      sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, float, Vec4&)>(&Vec4::SmoothStep),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, float)>(&Vec4::SmoothStep)
    ));
    Vec4Type.set_function("Barycentric",     sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, const Vec4&, float, float, Vec4&)>(&Vec4::Barycentric),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, const Vec4&, float, float)>(&Vec4::Barycentric)
    ));
    Vec4Type.set_function("CatmullRom",      sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, const Vec4&, const Vec4&, float, Vec4&)>(&Vec4::CatmullRom),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, const Vec4&, const Vec4&, float)>(&Vec4::CatmullRom)
    ));
    Vec4Type.set_function("Hermite",         sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, const Vec4&, const Vec4&, float, Vec4&)>(&Vec4::Hermite),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, const Vec4&, const Vec4&, float)>(&Vec4::Hermite)
    ));
    Vec4Type.set_function("Reflect",         sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, Vec4&)>(&Vec4::Reflect),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&)>(&Vec4::Reflect)
    ));
    Vec4Type.set_function("Refract",         sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, float, Vec4&)>(&Vec4::Refract),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, float)>(&Vec4::Refract)
    ));

    // Transform overloads
    Vec4Type.set_function("Transform",       sol::overload(
        static_cast<void (*)(const Vec2&, const Quat&, Vec4&)>(&Vec4::Transform),
        static_cast<Vec4 (*)(const Vec2&, const Quat&)>(&Vec4::Transform),
        static_cast<void (*)(const Vec3&, const Quat&, Vec4&)>(&Vec4::Transform),
        static_cast<Vec4 (*)(const Vec3&, const Quat&)>(&Vec4::Transform),
        static_cast<void (*)(const Vec4&, const Quat&, Vec4&)>(&Vec4::Transform),
        static_cast<Vec4 (*)(const Vec4&, const Quat&)>(&Vec4::Transform),
        static_cast<void (*)(const Vec4&, const Mat4&, Vec4&)>(&Vec4::Transform),
        static_cast<Vec4 (*)(const Vec4&, const Mat4&)>(&Vec4::Transform)
    ));

    // Static constants
    luaState["Vec4Zero"] = Vec4::Zero;
    luaState["Vec4One"] = Vec4::One;
    luaState["Vec4UnitX"] = Vec4::UnitX;
    luaState["Vec4UnitY"] = Vec4::UnitY;
    luaState["Vec4UnitZ"] = Vec4::UnitZ;
    luaState["Vec4UnitW"] = Vec4::UnitW;
}

#endif
