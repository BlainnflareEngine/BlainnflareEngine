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
        sol::meta_function::addition, [](const Vec4& a, const Vec4& b) { return a + b; },
        sol::meta_function::subtraction, [](const Vec4& a, const Vec4& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const Vec4 &a, const Vec4 &b) { return a * b; },
            [](const Vec4 &v, float s) { return v * s; },
            [](float s, const Vec4 &v) { return s * v; }
            ),
        sol::meta_function::division, sol::overload(
            [](const Vec4 &a, const Vec4 &b) { return a / b; },
            [](const Vec4 &v, float s) { return v / s; },
            [](float s, const Vec4 &v) { return s / v; }
            ),
        sol::meta_function::unary_minus, [](const Vec4& a) { return -a; },
        sol::meta_function::equal_to, [](const Vec4& a, const Vec4& b) { return a == b; }
    );

    // Members
    Vec4Type["x"] = &Vec4::x;
    Vec4Type["y"] = &Vec4::y;
    Vec4Type["z"] = &Vec4::z;
    Vec4Type["w"] = &Vec4::w;

    // Call operator for convenience Vec4(x,y,z,w)
    Vec4Type[sol::meta_function::call] = [](float x, float y, float z, float w) { return Vec4(x, y, z, w); };

    // Instance methods
    Vec4Type["Length"] = static_cast<float (Vec4::*)() const>(&Vec4::Length);
    Vec4Type["LengthSquared"] = static_cast<float (Vec4::*)() const>(&Vec4::LengthSquared);
    Vec4Type["Normalize"] = static_cast<void (Vec4::*)()>(&Vec4::Normalize);
    Vec4Type["Dot"] = static_cast<float (Vec4::*)(const Vec4&) const>(&Vec4::Dot);
    Vec4Type["Cross"] = sol::overload(
        static_cast<void (Vec4::*)(const Vec4&, const Vec4&, Vec4&) const>(&Vec4::Cross),
        static_cast<Vec4 (Vec4::*)(const Vec4&, const Vec4&) const>(&Vec4::Cross)
    );
    Vec4Type["InBounds"] = &Vec4::InBounds;
    Vec4Type["Clamp"] = sol::overload(
        static_cast<void (Vec4::*)(const Vec4&, const Vec4&)>(&Vec4::Clamp),
        static_cast<void (Vec4::*)(const Vec4&, const Vec4&, Vec4&) const>(&Vec4::Clamp)
    );

    // Static methods
    Vec4Type["Distance"] = static_cast<float (*)(const Vec4&, const Vec4&)>(&Vec4::Distance);
    Vec4Type["DistanceSquared"] = static_cast<float (*)(const Vec4&, const Vec4&)>(&Vec4::DistanceSquared);

    Vec4Type["Min"] = sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, Vec4&)>(&Vec4::Min),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&)>(&Vec4::Min)
    );
    Vec4Type["Max"] = sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, Vec4&)>(&Vec4::Max),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&)>(&Vec4::Max)
    );
    Vec4Type["Lerp"] = sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, float, Vec4&)>(&Vec4::Lerp),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, float)>(&Vec4::Lerp)
    );
    Vec4Type["SmoothStep"] = sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, float, Vec4&)>(&Vec4::SmoothStep),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, float)>(&Vec4::SmoothStep)
    );
    Vec4Type["Barycentric"] = sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, const Vec4&, float, float, Vec4&)>(&Vec4::Barycentric),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, const Vec4&, float, float)>(&Vec4::Barycentric)
    );
    Vec4Type["CatmullRom"] = sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, const Vec4&, const Vec4&, float, Vec4&)>(&Vec4::CatmullRom),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, const Vec4&, const Vec4&, float)>(&Vec4::CatmullRom)
    );
    Vec4Type["Hermite"] = sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, const Vec4&, const Vec4&, float, Vec4&)>(&Vec4::Hermite),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, const Vec4&, const Vec4&, float)>(&Vec4::Hermite)
    );
    Vec4Type["Reflect"] = sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, Vec4&)>(&Vec4::Reflect),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&)>(&Vec4::Reflect)
    );
    Vec4Type["Refract"] = sol::overload(
        static_cast<void (*)(const Vec4&, const Vec4&, float, Vec4&)>(&Vec4::Refract),
        static_cast<Vec4 (*)(const Vec4&, const Vec4&, float)>(&Vec4::Refract)
    );

    // Transform overloads
    Vec4Type["Transform"] = sol::overload(
        static_cast<void (*)(const Vec2&, const Quat&, Vec4&)>(&Vec4::Transform),
        static_cast<Vec4 (*)(const Vec2&, const Quat&)>(&Vec4::Transform),
        static_cast<void (*)(const Vec3&, const Quat&, Vec4&)>(&Vec4::Transform),
        static_cast<Vec4 (*)(const Vec3&, const Quat&)>(&Vec4::Transform),
        static_cast<void (*)(const Vec4&, const Quat&, Vec4&)>(&Vec4::Transform),
        static_cast<Vec4 (*)(const Vec4&, const Quat&)>(&Vec4::Transform),
        static_cast<void (*)(const Vec4&, const Mat4&, Vec4&)>(&Vec4::Transform),
        static_cast<Vec4 (*)(const Vec4&, const Mat4&)>(&Vec4::Transform)
    );

    // Static constants
    luaState["Vec4Zero"] = Vec4::Zero;
    luaState["Vec4One"] = Vec4::One;
    luaState["Vec4UnitX"] = Vec4::UnitX;
    luaState["Vec4UnitY"] = Vec4::UnitY;
    luaState["Vec4UnitZ"] = Vec4::UnitZ;
    luaState["Vec4UnitW"] = Vec4::UnitW;
}

#endif