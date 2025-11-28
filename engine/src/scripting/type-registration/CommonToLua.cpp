#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"
#include "spdlog/spdlog.h"
#include "subsystems/Log.h"

using namespace Blainn;

static Quat Quat_GetNormalized(const Quat &q)
{
    Quat out;
    q.Normalize(out);
    return out;
}

static Quat Quat_Conjugate(const Quat &q)
{
    Quat out;
    q.Conjugate(out);
    return out;
}

static Quat Quat_Inverse(const Quat &q)
{
    Quat out;
    q.Inverse(out);
    return out;
}

void Blainn::RegisterCommonTypes(sol::state &luaState)
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

    Vec2Type["Length"] = &Vec2::Length;
    Vec2Type["LengthSquared"] = &Vec2::LengthSquared;
    Vec2Type["Normalize"] = static_cast<void (Vec2::*)()>(&Vec2::Normalize);
    Vec2Type["Dot"] = &Vec2::Dot;

    Vec2Type["Distance"] = &Vec2::Distance;
    Vec2Type["Clamp"] = static_cast<void (Vec2::*)(const Vec2 &, const Vec2 &)>(&Vec2::Clamp);
    Vec2Type["Lerp"] = static_cast<Vec2 (*)(const Vec2 &, const Vec2 &, float)>(&Vec2::Lerp);

    // Register Vec3
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

    Vec3Type["Length"] = static_cast<float (Vec3::*)() const>(&Vec3::Length);
    Vec3Type["LengthSquared"] = static_cast<float (Vec3::*)() const>(&Vec3::LengthSquared);
    Vec3Type["Normalize"] = static_cast<void (Vec3::*)()>(&Vec3::Normalize);
    Vec3Type["Dot"] = static_cast<float (Vec3::*)(const Vec3 &) const>(&Vec3::Dot);
    Vec3Type["Cross"] = static_cast<Vec3 (Vec3::*)(const Vec3 &) const>(&Vec3::Cross);
    Vec3Type["Distance"] = static_cast<float (*)(const Vec3 &, const Vec3 &)>(&Vec3::Distance);
    Vec3Type["Clamp"] = static_cast<void (Vec3::*)(const Vec3 &, const Vec3 &)>(&Vec3::Clamp);
    Vec3Type["Lerp"] = static_cast<Vec3 (*)(const Vec3 &, const Vec3 &, float)>(&Vec3::Lerp);

    // Register Vec4
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

    Vec4Type["Length"] = static_cast<float (Vec4::*)() const>(&Vec4::Length);
    Vec4Type["LengthSquared"] = static_cast<float (Vec4::*)() const>(&Vec4::LengthSquared);
    Vec4Type["Normalize"] = static_cast<void (Vec4::*)()>(&Vec4::Normalize);
    Vec4Type["Dot"] = static_cast<float (Vec4::*)(const Vec4 &) const>(&Vec4::Dot);
    Vec4Type["Distance"] = static_cast<float (*)(const Vec4 &, const Vec4 &)>(&Vec4::Distance);
    Vec4Type["Lerp"] = static_cast<Vec4 (*)(const Vec4 &, const Vec4 &, float)>(&Vec4::Lerp);

    // Register Quaternion (Quat)
    sol::usertype<Quat> QuatType = luaState.new_usertype<Quat>(
        "Quat", sol::constructors<Quat(), Quat(float, float, float, float)>(), sol::meta_function::multiplication,
        [](const Quat &a, const Quat &b) { return a * b; }, sol::meta_function::equal_to,
        [](const Quat &a, const Quat &b) { return a == b; });

    QuatType["x"] = &Quat::x;
    QuatType["y"] = &Quat::y;
    QuatType["z"] = &Quat::z;
    QuatType["w"] = &Quat::w;

    // Provide quaternion helper accessors and common helpers
    QuatType["Normalize"] = static_cast<void (Quat::*)()>(&Quat::Normalize);
    QuatType["GetNormalized"] = &Quat_GetNormalized;
    QuatType["Dot"] = static_cast<float (Quat::*)(const Quat &) const>(&Quat::Dot);
    QuatType["Conjugate"] = &Quat_Conjugate;
    QuatType["Inverse"] = &Quat_Inverse;

    // Register Mat4 (Matrix)
    sol::usertype<Mat4> Mat4Type =
        luaState.new_usertype<Mat4>("Mat4", sol::constructors<Mat4()>(), sol::meta_function::multiplication,
                                    [](const Mat4 &a, const Mat4 &b) { return a * b; });

    // Add Mat4 factory helpers and instance helpers
    Mat4Type["CreateTranslation"] = static_cast<Mat4 (*)(const Vec3 &)>(&Mat4::CreateTranslation);
    Mat4Type["CreateScaleVec"] = static_cast<Mat4 (*)(const Vec3 &)>(&Mat4::CreateScale);
    Mat4Type["CreateScale"] = static_cast<Mat4 (*)(float, float, float)>(&Mat4::CreateScale);
    Mat4Type["CreateScaleUniform"] = static_cast<Mat4 (*)(float)>(&Mat4::CreateScale);
    Mat4Type["CreateRotationX"] = static_cast<Mat4 (*)(float)>(&Mat4::CreateRotationX);
    Mat4Type["CreateRotationY"] = static_cast<Mat4 (*)(float)>(&Mat4::CreateRotationY);
    Mat4Type["CreateRotationZ"] = static_cast<Mat4 (*)(float)>(&Mat4::CreateRotationZ);
    Mat4Type["CreateFromQuaternion"] = static_cast<Mat4 (*)(const Quat &)>(&Mat4::CreateFromQuaternion);

    Mat4Type["Transpose"] = static_cast<Mat4 (Mat4::*)() const>(&Mat4::Transpose);
    Mat4Type["Invert"] = static_cast<Mat4 (Mat4::*)() const>(&Mat4::Invert);
    Mat4Type["ToEuler"] = static_cast<Vec3 (Mat4::*)() const>(&Mat4::ToEuler);
    // Register the identity matrix as a convenience in the global Lua state
    luaState["Mat4Identity"] = sol::make_object(luaState, Mat4::Identity);

    // Register Plane
    sol::usertype<Plane> PlaneType =
        luaState.new_usertype<Plane>("Plane", sol::constructors<Plane()>(), sol::meta_function::equal_to,
                                     [](const Plane &a, const Plane &b) { return a == b; });

    // Bind Plane properties via the accessor methods
    PlaneType["Normal"] = [](const Plane &p) { return p.Normal(); };
    PlaneType["SetNormal"] = [](Plane &p, const Vec3 &v) { p.Normal(v); };
    PlaneType["D"] = [](const Plane &p) { return p.D(); };
    PlaneType["SetD"] = [](Plane &p, float d) { p.D(d); };

    // Register Rect
    sol::usertype<Rect> RectType =
        luaState.new_usertype<Rect>("Rect", sol::constructors<Rect(), Rect(float, float, float, float)>(),
                                    sol::meta_function::equal_to, [](const Rect &a, const Rect &b) { return a == b; });

    // Bind Rect fields (SimpleMath Rectangle members are x,y,width,height)
    RectType["x"] = &Rect::x;
    RectType["y"] = &Rect::y;
    RectType["width"] = &Rect::width;
    RectType["height"] = &Rect::height;

    // Register Ray
    sol::usertype<Ray> RayType =
        luaState.new_usertype<Ray>("Ray", sol::constructors<Ray(), Ray(const Vec3 &, const Vec3 &)>(),
                                   sol::meta_function::equal_to, [](const Ray &a, const Ray &b) { return a == b; });

    // Ray fields: position and direction
    RayType["position"] = &Ray::position;
    RayType["direction"] = &Ray::direction;

    // Register Viewport
    sol::usertype<Viewport> ViewportType = luaState.new_usertype<Viewport>(
        "Viewport", sol::constructors<Viewport(), Viewport(int, int, int, int)>(), sol::meta_function::equal_to,
        [](const Viewport &a, const Viewport &b) { return a == b; });

    // Viewport fields: x,y,width,height,minDepth,maxDepth
    ViewportType["x"] = &Viewport::x;
    ViewportType["y"] = &Viewport::y;
    ViewportType["width"] = &Viewport::width;
    ViewportType["height"] = &Viewport::height;
    ViewportType["minDepth"] = &Viewport::minDepth;
    ViewportType["maxDepth"] = &Viewport::maxDepth;

    // Register Color
    sol::usertype<Color> ColorType = luaState.new_usertype<Color>(
        "Color", sol::constructors<Color(), Color(float, float, float, float)>(), sol::meta_function::equal_to,
        [](const Color &a, const Color &b) { return a == b; });

    // Color accessors and helpers
    ColorType["R"] =
        sol::property(static_cast<float (Color::*)() const>(&Color::R), static_cast<void (Color::*)(float)>(&Color::R));
    ColorType["G"] =
        sol::property(static_cast<float (Color::*)() const>(&Color::G), static_cast<void (Color::*)(float)>(&Color::G));
    ColorType["B"] =
        sol::property(static_cast<float (Color::*)() const>(&Color::B), static_cast<void (Color::*)(float)>(&Color::B));
    ColorType["A"] =
        sol::property(static_cast<float (Color::*)() const>(&Color::A), static_cast<void (Color::*)(float)>(&Color::A));
    ColorType["ToVector3"] = static_cast<Vec3 (Color::*)() const>(&Color::ToVector3);
    ColorType["ToVector4"] = static_cast<Vec4 (Color::*)() const>(&Color::ToVector4);

    // Register Log functions
    sol::table logTable = luaState.create_table();

    logTable.set_function("Trace",
                          [](const std::string &msg)
                          {
                              auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
                              if (logger) logger->trace(msg);
                          });

    logTable.set_function("Debug",
                          [](const std::string &msg)
                          {
                              auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
                              if (logger) logger->debug(msg);
                          });

    logTable.set_function("Info",
                          [](const std::string &msg)
                          {
                              auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
                              if (logger) logger->info(msg);
                          });

    logTable.set_function("Warn",
                          [](const std::string &msg)
                          {
                              auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
                              if (logger) logger->warn(msg);
                          });

    logTable.set_function("Error",
                          [](const std::string &msg)
                          {
                              auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
                              if (logger) logger->error(msg);
                          });

    logTable.set_function("Fatal",
                          [](const std::string &msg)
                          {
                              auto logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);
                              if (logger) logger->critical(msg);
                          });

    luaState["Log"] = logTable;
}