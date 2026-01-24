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
    QuatType["Normalize"]  = static_cast<void (Quat::*)()>(&Quat::Normalize);
    QuatType["Conjugate"]  = static_cast<void (Quat::*)()>(&Quat::Conjugate);

    // instance methods (non-mutating)
    QuatType["Length"]         = &Quat::Length;
    QuatType["LengthSquared"]  = &Quat::LengthSquared;
    QuatType["Dot"]            = static_cast<float (Quat::*)(const Quat&) const>(&Quat::Dot);
    QuatType["ToEuler"]        = &Quat::ToEuler;

    QuatType["Normalized"] = &Quat_Normalized;
    QuatType["Conjugated"] = &Quat_Conjugated;
    QuatType["Inversed"]   = &Quat_Inversed;

    // static constructors
    QuatType["Identity"] = sol::var(Quat::Identity);

    QuatType["FromAxisAngle"] = &Quat::CreateFromAxisAngle;

    QuatType["FromYawPitchRoll"] = sol::overload(
        static_cast<Quat (*)(float, float, float)>(&Quat::CreateFromYawPitchRoll),
        static_cast<Quat (*)(const Vec3&)>(&Quat::CreateFromYawPitchRoll)
    );

    QuatType["FromRotationMatrix"] = &Quat::CreateFromRotationMatrix;

    QuatType["FromToRotation"] = static_cast<Quat (*)(const Vec3&, const Vec3&)>(&Quat::FromToRotation);

    QuatType["LookRotation"] = static_cast<Quat (*)(const Vec3&, const Vec3&)>(&Quat::LookRotation);

    // interpolation
    QuatType["Lerp"]  = &Quat_Lerp;
    QuatType["Slerp"] = &Quat_Slerp;

    // composition
    QuatType["Concatenate"] = &Quat_Concatenate;

    // metrics
    QuatType["Angle"] = &Quat::Angle;
}
#endif
