#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "subsystems/PhysicsSubsystem.h"
#include "physics/BodyUpdater.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterPhysicsBodyUpdater(sol::state &luaState, sol::table &physicsTable)
{
    physicsTable.set_function("GetBodyUpdater",
                              [&luaState](Entity e)
                              {
                                  sol::state_view lua(luaState);
                                  sol::table tbl = lua.create_table();

                                  tbl.set_function("SetPosition",
                                                   [e](const Vec3 &p, int activationInt)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetPosition(p, static_cast<EActivation>(activationInt));
                                                   });
                                  tbl.set_function("SetRotation",
                                                   [e](const Quat &r, int activationInt)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetRotation(r, static_cast<EActivation>(activationInt));
                                                   });
                                  tbl.set_function("SetScale",
                                                   [e](const Vec3 &s, const Vec3 &prev)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetScale(s, prev);
                                                   });
                                  tbl.set_function("SetVelocity",
                                                   [e](const Vec3 &v)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetVelocity(v);
                                                   });
                                  tbl.set_function("SetMaxLinearVelocity",
                                                   [e](float v)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetMaxLinearVelocity(v);
                                                   });
                                  tbl.set_function("SetAngularVelocity",
                                                   [e](const Vec3 &v)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetAngularVelocity(v);
                                                   });
                                  tbl.set_function("SetMaxAngularVelocity",
                                                   [e](float v)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetMaxAngularVelocity(v);
                                                   });
                                  tbl.set_function("SetGravityFactor",
                                                   [e](float g)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetGravityFactor(g);
                                                   });
                                  tbl.set_function("SetObjectLayer",
                                                   [e](int layer)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetObjectLayer(static_cast<JPH::ObjectLayer>(layer));
                                                   });
                                  tbl.set_function("AddVelocity",
                                                   [e](const Vec3 &d)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.AddVelocity(d);
                                                   });
                                  tbl.set_function("AddImpulse",
                                                   [e](const Vec3 &i)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.AddImpulse(i);
                                                   });
                                  tbl.set_function("AddAngularImpulse",
                                                   [e](const Vec3 &i)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.AddAngularImpulse(i);
                                                   });
                                  tbl.set_function("AddForce",
                                                   [e](const Vec3 &f)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.AddForce(f);
                                                   });
                                  tbl.set_function("ReplaceBodyShape",
                                                   [e](ShapeCreationSettings &s, int activationInt)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.ReplaceBodyShape(s, static_cast<EActivation>(activationInt));
                                                   });
                                  tbl.set_function("SetMotionType",
                                                   [e](int m, int activationInt)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetMotionType(static_cast<PhysicsComponentMotionType>(m),
                                                                       static_cast<EActivation>(activationInt));
                                                   });
                                  tbl.set_function("SetSphereShapeSettings",
                                                   [e](float radius)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetSphereShapeSettings(radius);
                                                   });
                                  tbl.set_function("SetBoxShapeSettings",
                                                   [e](const Vec3 &halfExtents)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetBoxShapeSettings(halfExtents);
                                                   });
                                  tbl.set_function("SetCapsuleShapeSettings",
                                                   [e](float halfHeight, float radius)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetCapsuleShapeSettings(halfHeight, radius);
                                                   });
                                  tbl.set_function("SetCylinderShapeSettings",
                                                   [e](float halfHeight, float radius)
                                                   {
                                                       auto u = PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetCylinderShapeSettings(halfHeight, radius);
                                                   });

                                  return tbl;
                              });
}

#endif