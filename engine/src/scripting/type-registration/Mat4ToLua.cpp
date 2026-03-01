//
// Created by WhoLeb on 16-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "pch.h"
#include "aliases.h"

#include "scripting/TypeRegistration.h"

#ifdef BLAINN_REGISTER_LUA_TYPES

using namespace Blainn;

static Mat4 Mat4_Transposed(const Mat4& m)
{
    return m.Transpose();
}

static Mat4 Mat4_Inverted(const Mat4& m)
{
    return m.Invert();
}

static std::tuple<Vec3, Quat, Vec3> Mat4_Decompose(Mat4& m)
{
    Vec3 scale, translation;
    Quat rotation;
    m.Decompose(scale, rotation, translation);
    return { scale, rotation, translation };
}

static Mat4 Mat4_Lerp(const Mat4& a, const Mat4& b, float t)
{
    return Mat4::Lerp(a, b, t);
}

static Mat4 Mat4_Transform(const Mat4& m, const Quat& q)
{
    return Mat4::Transform(m, q);
}

void Blainn::RegisterMat4Type(sol::state &luaState)
{
    auto Mat4Type = luaState.new_usertype<Mat4>(
        "Mat4",
        sol::constructors<
            Mat4(),
            Mat4(
                float, float, float, float,
                float, float, float, float,
                float, float, float, float,
                float, float, float, float
            )
        >()
    );

    // operators
    Mat4Type[sol::meta_function::equal_to] = &Mat4::operator==;
    Mat4Type[sol::meta_function::unary_minus] = [](const Mat4& m) { return -m; };

    Mat4Type[sol::meta_function::addition] =
        [](const Mat4& a, const Mat4& b) { return a + b; };

    Mat4Type[sol::meta_function::subtraction] =
        [](const Mat4& a, const Mat4& b) { return a - b; };

    Mat4Type[sol::meta_function::multiplication] = sol::overload(
        [](const Mat4& a, const Mat4& b) { return a * b; },
        [](const Mat4& m, float s) { return m * s; },
        [](float s, const Mat4& m) { return s * m; }
    );

    Mat4Type[sol::meta_function::division] = sol::overload(
        [](const Mat4& m, float s) { return m / s; },
        [](const Mat4& a, const Mat4& b) { return a / b; }
    );

    // properties (direction + translation)
    Mat4Type["Right"]       = sol::property(
        static_cast<Vec3 (Mat4::*)() const>(&Mat4::Right),
        static_cast<void (Mat4::*)(const Vec3&)>(&Mat4::Right));

    Mat4Type["Up"]          = sol::property(
        static_cast<Vec3 (Mat4::*)() const>(&Mat4::Right),
        static_cast<void (Mat4::*)(const Vec3&)>(&Mat4::Right));
    Mat4Type["Forward"]     = sol::property(
        static_cast<Vec3 (Mat4::*)() const>(&Mat4::Backward),
        static_cast<void (Mat4::*)(const Vec3&)>(&Mat4::Backward));
    Mat4Type["Translation"] = sol::property(
        static_cast<Vec3 (Mat4::*)() const>(&Mat4::Translation),
        static_cast<void (Mat4::*)(const Vec3&)>(&Mat4::Translation));

    // metrics
    Mat4Type.set_function("Determinant", &Mat4::Determinant);

    // instance methods (non-mutating)
    Mat4Type.set_function("Transpose",   &Mat4_Transposed);
    Mat4Type.set_function("Invert",      &Mat4_Inverted);
    Mat4Type.set_function("ToEuler",     &Mat4::ToEuler);

    Mat4Type.set_function("Decompose",   &Mat4_Decompose);

    // static factories
    Mat4Type["Identity"] = sol::var(Mat4::Identity);

    Mat4Type.set_function("Translation", sol::overload(
        static_cast<Mat4 (*)(const Vec3&)>(&Mat4::CreateTranslation),
        static_cast<Mat4 (*)(float, float, float)>(&Mat4::CreateTranslation)
    ));

    Mat4Type.set_function("Scale",       sol::overload(
        static_cast<Mat4 (*)(const Vec3&)>(&Mat4::CreateScale),
        static_cast<Mat4 (*)(float, float, float)>(&Mat4::CreateScale),
        static_cast<Mat4 (*)(float)>(&Mat4::CreateScale)
    ));

    Mat4Type.set_function("RotationX",   &Mat4::CreateRotationX);
    Mat4Type.set_function("RotationY",   &Mat4::CreateRotationY);
    Mat4Type.set_function("RotationZ",   &Mat4::CreateRotationZ);

    Mat4Type.set_function("FromAxisAngle",
        &Mat4::CreateFromAxisAngle);

    Mat4Type.set_function("FromQuaternion",
        &Mat4::CreateFromQuaternion);

    Mat4Type.set_function("FromYawPitchRoll", sol::overload(
        static_cast<Mat4 (*)(float, float, float)>(&Mat4::CreateFromYawPitchRoll),
        static_cast<Mat4 (*)(const Vec3&)>(&Mat4::CreateFromYawPitchRoll)
    ));

    Mat4Type.set_function("LookAt",      &Mat4::CreateLookAt);
    Mat4Type.set_function("World",       &Mat4::CreateWorld);

    Mat4Type.set_function("PerspectiveFov",
        &Mat4::CreatePerspectiveFieldOfView);

    Mat4Type.set_function("Perspective",
        &Mat4::CreatePerspective);

    Mat4Type.set_function("Orthographic",
        &Mat4::CreateOrthographic);

    // interpolation / transform
    Mat4Type.set_function("Lerp",        &Mat4_Lerp);
    Mat4Type.set_function("Transform",   &Mat4_Transform);
}
#endif
