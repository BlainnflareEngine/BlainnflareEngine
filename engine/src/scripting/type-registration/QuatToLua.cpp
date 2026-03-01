//
// Created by WhoLeb on 16-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "pch.h"
#include "aliases.h"

#include "scripting/TypeRegistration.h"

#ifdef BLAINN_REGISTER_LUA_TYPES
using namespace Blainn;

static Quat Quat_Normalized(const Quat& q)
{
    Quat out;
    q.Normalize(out);
    return out;
}

static Quat Quat_Conjugated(const Quat& q)
{
    Quat out;
    q.Conjugate(out);
    return out;
}

static Quat Quat_Inversed(const Quat& q)
{
    Quat out;
    q.Inverse(out);
    return out;
}

static Quat Quat_Lerp(const Quat& a, const Quat& b, float t)
{
    return Quat::Lerp(a, b, t);
}

static Quat Quat_Slerp(const Quat& a, const Quat& b, float t)
{
    return Quat::Slerp(a, b, t);
}

static Quat Quat_Concatenate(const Quat& a, const Quat& b)
{
    return Quat::Concatenate(a, b);
}

void Blainn::RegisterQuatType(sol::state &luaState)
{
    // Register Quaternion (Quat)
    auto QuatType = luaState.new_usertype<Quat>(
        "Quat",
        sol::constructors<
            Quat(),
            Quat(float, float, float, float)
        >()
    );

    // data
    QuatType["x"] = &Quat::x;
    QuatType["y"] = &Quat::y;
    QuatType["z"] = &Quat::z;
    QuatType["w"] = &Quat::w;

    // operators
    QuatType[sol::meta_function::equal_to] = &Quat::operator==;
    QuatType[sol::meta_function::unary_minus] = [](const Quat& q) { return -q; };

    QuatType[sol::meta_function::addition] = [](const Quat& a, const Quat& b) { return a + b; };
    QuatType[sol::meta_function::subtraction] = [](const Quat& a, const Quat& b) { return a - b; };
    QuatType[sol::meta_function::multiplication] = sol::overload(
        [](const Quat& a, const Quat& b) { return a * b; },
        [](const Quat& q, float s) { return q * s; },
        [](float s, const Quat& q) { return s * q; }
    );
    QuatType[sol::meta_function::division] =
        [](const Quat& a, const Quat& b) { return a / b; };

    // instance methods (mutating)
    QuatType.set_function("Normalize",     static_cast<void (Quat::*)()>(&Quat::Normalize));
    QuatType.set_function("Conjugate",     static_cast<void (Quat::*)()>(&Quat::Conjugate));

    // instance methods (non-mutating)
    QuatType.set_function("Length",        &Quat::Length);
    QuatType.set_function("LengthSquared", &Quat::LengthSquared);
    QuatType.set_function("Dot",           static_cast<float (Quat::*)(const Quat&) const>(&Quat::Dot));
    QuatType.set_function("ToEuler",       &Quat::ToEuler);

    QuatType.set_function("Normalized",    &Quat_Normalized);
    QuatType.set_function("Conjugated",    &Quat_Conjugated);
    QuatType.set_function("Inversed",      &Quat_Inversed);

    // static constructors
    QuatType["Identity"] = sol::var(Quat::Identity);

    QuatType.set_function("FromAxisAngle", &Quat::CreateFromAxisAngle);

    QuatType.set_function("FromYawPitchRoll", sol::overload(
        static_cast<Quat (*)(float, float, float)>(&Quat::CreateFromYawPitchRoll),
        static_cast<Quat (*)(const Vec3&)>(&Quat::CreateFromYawPitchRoll)
    ));

    QuatType.set_function("FromRotationMatrix", &Quat::CreateFromRotationMatrix);

    QuatType.set_function("FromToRotation", static_cast<Quat (*)(const Vec3&, const Vec3&)>(&Quat::FromToRotation));

    QuatType.set_function("LookRotation",   static_cast<Quat (*)(const Vec3&, const Vec3&)>(&Quat::LookRotation));

    // interpolation
    QuatType.set_function("Lerp",          &Quat_Lerp);
    QuatType.set_function("Slerp",         &Quat_Slerp);

    // composition
    QuatType.set_function("Concatenate",   &Quat_Concatenate);

    // metrics
    QuatType.set_function("Angle",         &Quat::Angle);
}
#endif
