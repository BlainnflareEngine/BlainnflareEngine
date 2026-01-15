#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "subsystems/PhysicsSubsystem.h"
#include "physics/BodyGetter.h"

using namespace Blainn;

// Lightweight wrapper for JPH::RefConst<JPH::Shape> to expose to Lua without triggering sol2 auto-comparisons
struct ShapeRef
{
    const JPH::Shape *ptr = nullptr;
    ShapeRef() = default;
    ShapeRef(const JPH::RefConst<JPH::Shape> &r)
        : ptr(r.GetPtr())
    {
    }
    ShapeRef(const JPH::Shape *p)
        : ptr(p)
    {
    }
    bool IsValid() const
    {
        return ptr != nullptr;
    }
    const JPH::Shape *Get() const
    {
        return ptr;
    }
    bool operator==(const ShapeRef &o) const
    {
        return ptr == o.ptr;
    }
};

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterPhysicsBodyGetter(sol::state &luaState, sol::table &physicsTable)
{
    // Register ShapeRef wrapper for shapes returned by BodyGetter
    sol::usertype<ShapeRef> ShapeRefType = luaState.new_usertype<ShapeRef>("ShapeRef", sol::constructors<ShapeRef()>());
    ShapeRefType.set_function("IsValid", &ShapeRef::IsValid);
    ShapeRefType.set_function("Get", &ShapeRef::Get);

    physicsTable.set_function("GetBodyGetter",
                              [&luaState](Entity e)
                              {
                                  sol::state_view lua(luaState);
                                  sol::table tbl = lua.create_table();

                                  tbl.set_function("GetPosition",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetPosition();
                                                   });
                                  tbl.set_function("GetRotation",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetRotation();
                                                   });
                                  tbl.set_function("GetShape",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return ShapeRef(g.GetShape());
                                                   });
                                  tbl.set_function("GetShapeType",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetShapeType();
                                                   });
                                  tbl.set_function("GetVelocity",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetVelocity();
                                                   });
                                  tbl.set_function("GetMaxLinearVelocity",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetMaxLinearVelocity();
                                                   });
                                  tbl.set_function("GetAngularVelocity",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetAngularVelocity();
                                                   });
                                  tbl.set_function("GetMaxAngularVelocity",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetMaxAngularVelocity();
                                                   });
                                  tbl.set_function("GetGravityFactor",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetGravityFactor();
                                                   });
                                  tbl.set_function("IsTrigger",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.isTrigger();
                                                   });
                                  tbl.set_function("GetObjectLayer",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetObjectLayer();
                                                   });
                                  tbl.set_function("GetMotionType",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetMotionType();
                                                   });
                                  tbl.set_function("GetSphereShapeRadius",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetSphereShapeRadius().value_or(0.0f);
                                                   });
                                  tbl.set_function("GetBoxShapeHalfExtents",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetBoxShapeHalfExtents().value_or(Vec3(0.0f));
                                                   });
                                  tbl.set_function("GetCylinderShapeHalfHeightAndRadius",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetCylinderShapeHalfHeightAndRadius();
                                                   });
                                  tbl.set_function("GetCapsuleShapeHalfHeightAndRadius",
                                                   [e]()
                                                   {
                                                       auto g = PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetCapsuleShapeHalfHeightAndRadius();
                                                   });

                                  return tbl;
                              });
}

#endif