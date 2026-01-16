//
// Created by WhoLeb on 16-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "pch.h"
#include "aliases.h"

#include "scripting/TypeRegistration.h"

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterMat4Type(sol::state &luaState)
{
    // Register Mat4 (Matrix)
    sol::usertype<Mat4> Mat4Type =
        luaState.new_usertype<Mat4>("Mat4", sol::constructors<Mat4()>(), sol::meta_function::multiplication,
                                    [](const Mat4 &a, const Mat4 &b) { return a * b; });

    auto toVec3 = [](const sol::object &o) -> Vec3
    {
        if (o.is<Vec3>()) return o.as<Vec3>();
        if (o.is<sol::table>())
        {
            sol::table t = o.as<sol::table>();
            float x = t["x"] ? t["x"] : 0.0f;
            float y = t["y"] ? t["y"] : 0.0f;
            float z = t["z"] ? t["z"] : 0.0f;
            return Vec3(x, y, z);
        }
        return Vec3::Zero;
    };

    // Add Mat4 factory helpers and instance helpers
    Mat4Type["CreateTranslation"] = [toVec3](const sol::object &o) { return Mat4::CreateTranslation(toVec3(o)); };
    Mat4Type["CreateScaleVec"] = [toVec3](const sol::object &o) { return Mat4::CreateScale(toVec3(o)); };
    Mat4Type["CreateScale"] = static_cast<Mat4 (*)(float, float, float)>(&Mat4::CreateScale);
    Mat4Type["CreateScaleUniform"] = static_cast<Mat4 (*)(float)>(&Mat4::CreateScale);
    Mat4Type["CreateRotationX"] = static_cast<Mat4 (*)(float)>(&Mat4::CreateRotationX);
    Mat4Type["CreateRotationY"] = static_cast<Mat4 (*)(float)>(&Mat4::CreateRotationY);
    Mat4Type["CreateRotationZ"] = static_cast<Mat4 (*)(float)>(&Mat4::CreateRotationZ);
    auto toQuat = [](const sol::object &o) -> Quat
    {
        if (o.is<Quat>()) return o.as<Quat>();
        if (o.is<sol::table>())
        {
            sol::table t = o.as<sol::table>();
            float x = t["x"] ? t["x"] : 0.0f;
            float y = t["y"] ? t["y"] : 0.0f;
            float z = t["z"] ? t["z"] : 0.0f;
            float w = t["w"] ? t["w"] : 0.0f;
            return Quat(w, x, y, z);
        }
        return Quat(1.f, 0.f, 0.f, 0.f);
    };

    Mat4Type["CreateFromQuaternion"] = [toQuat](const sol::object &o) { return Mat4::CreateFromQuaternion(toQuat(o)); };

    Mat4Type["Transpose"] = static_cast<Mat4 (Mat4::*)() const>(&Mat4::Transpose);
    Mat4Type["Invert"] = static_cast<Mat4 (Mat4::*)() const>(&Mat4::Invert);
    Mat4Type["ToEuler"] = static_cast<Vec3 (Mat4::*)() const>(&Mat4::ToEuler);
    // Register the identity matrix as a convenience in the global Lua state
    luaState["Mat4Identity"] = sol::make_object(luaState, Mat4::Identity);
}
#endif
