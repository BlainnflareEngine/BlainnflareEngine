#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"
#include "spdlog/spdlog.h"
#include "subsystems/Log.h"
#include "subsystems/ScriptingSubsystem.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

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

void Blainn::RegisterCommonTypes(sol::state &luaState)
{
    RegisterVectorTypes(luaState);

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

    RegisterUselessSimpleMathTypes(luaState);

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

#endif