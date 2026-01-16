//
// Created by WhoLeb on 16-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "pch.h"
#include "aliases.h"

#include "scripting/TypeRegistration.h"

#ifdef BLAINN_REGISTER_LUA_TYPES
using namespace Blainn;

static Quat QuatGetNormalized(const Quat &q)
{
    Quat out;
    q.Normalize(out);
    return out;
}

static Quat QuatConjugate(const Quat &q)
{
    Quat out;
    q.Conjugate(out);
    return out;
}

static Quat QuatInverse(const Quat &q)
{
    Quat out;
    q.Inverse(out);
    return out;
}

void Blainn::RegisterQuatType(sol::state &luaState)
{
    // Register Quaternion (Quat)
    sol::usertype<Quat> QuatType = luaState.new_usertype<Quat>(
        "Quat", sol::constructors<Quat(), Quat(float, float, float, float)>(), sol::meta_function::multiplication,
        [](const Quat &a, const Quat &b) { return a * b; }, sol::meta_function::equal_to,
        [](const Quat &a, const Quat &b) { return a == b; });

    QuatType["x"] = &Quat::x;
    QuatType["y"] = &Quat::y;
    QuatType["z"] = &Quat::z;
    QuatType["w"] = &Quat::w;

    // Make Quat callable: Quat(x,y,z,w)
    QuatType[sol::meta_function::call] = [](float x, float y, float z, float w) { return Quat(x, y, z, w); };

    // Provide quaternion helper accessors and common helpers
    QuatType["Normalize"] = static_cast<void (Quat::*)()>(&Quat::Normalize);
    QuatType["GetNormalized"] = &QuatGetNormalized;
    QuatType["Dot"] = static_cast<float (Quat::*)(const Quat &) const>(&Quat::Dot);
    QuatType["Conjugate"] = &QuatConjugate;
    QuatType["Inverse"] = &QuatInverse;
}
#endif
