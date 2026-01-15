#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterVectorTypes(sol::state &luaState)
{
    sol::usertype<Vec2> Vec2Type = luaState.new_usertype<Vec2>(
        "Vec2", sol::constructors<Vec2(), Vec2(float, float)>(), sol::meta_function::subtraction,
        [](const Vec2 &a, const Vec2 &b) { return a - b; }, sol::meta_function::addition,
        [](const Vec2 &a, const Vec2 &b) { return a + b; }, sol::meta_function::unary_minus, [](const Vec2 &a)
        { return -a; }, sol::meta_function::multiplication, [](const Vec2 &a, float s) { return a * s; },
        sol::meta_function::division, [](const Vec2 &a, float s) { return a / s; }, sol::meta_function::equal_to,
        [](const Vec2 &a, const Vec2 &b) { return a == b; });

    Vec2Type["x"] = &Vec2::x;
    Vec2Type["y"] = &Vec2::y;

    Vec2Type[sol::meta_function::call] = [](float x, float y) { return Vec2(x, y); };

    Vec2Type["Length"] = &Vec2::Length;
    Vec2Type["LengthSquared"] = &Vec2::LengthSquared;
    Vec2Type["Normalize"] = static_cast<void (Vec2::*)()>(&Vec2::Normalize);
    Vec2Type["Dot"] = &Vec2::Dot;

    Vec2Type["Distance"] = &Vec2::Distance;
    Vec2Type["Clamp"] = static_cast<void (Vec2::*)(const Vec2 &, const Vec2 &)>(&Vec2::Clamp);
    Vec2Type["Lerp"] = static_cast<Vec2 (*)(const Vec2 &, const Vec2 &, float)>(&Vec2::Lerp);

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