#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "physics/BodyGetter.h"
#include "physics/BodyUpdater.h"
#include "physics/PhysicsCreationSettings.h"
#include "physics/RayCastResult.h"
#include "physics/PhysicsEvents.h"
#include "physics/PhysicsTypes.h"
#include "physics/Layers.h"
#include "subsystems/PhysicsSubsystem.h"
#include "subsystems/ScriptingSubsystem.h"
#include <atomic>
#include <unordered_map>

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

// Listener handle storage for Lua -> PhysicsSubsystem event listeners
static inline std::unordered_map<uint64_t, Blainn::PhysicsSubsystem::PhysicsEventHandle> s_listenerHandles;
static inline std::atomic<uint64_t> s_nextListenerId{1};

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

void Blainn::RegisterPhysicsTypes(sol::state &luaState)
{
    // Enums
    // Expose physics event enum so Lua can listen for collision events
    luaState.new_enum<true>("PhysicsEventType", "CollisionStarted", PhysicsEventType::CollisionStarted,
                            "CollisionEnded", PhysicsEventType::CollisionEnded);

    luaState.new_enum<true>("ComponentShapeType", "Sphere", ComponentShapeType::Sphere, "Box", ComponentShapeType::Box,
                            "Capsule", ComponentShapeType::Capsule, "Cylinder", ComponentShapeType::Cylinder, "Empty",
                            ComponentShapeType::Empty);

    luaState.new_enum<true>("PhysicsComponentMotionType", "Static", PhysicsComponentMotionType::Static, "Kinematic",
                            PhysicsComponentMotionType::Kinematic, "Dynamic", PhysicsComponentMotionType::Dynamic);

    luaState.new_enum<true>("EActivation", "DontActivate", EActivation::DontActivate, "Activate",
                            EActivation::Activate);

    // ShapeCreationSettings
    sol::usertype<ShapeCreationSettings> ShapeSettingsType = luaState.new_usertype<ShapeCreationSettings>(
        "ShapeCreationSettings", sol::constructors<ShapeCreationSettings(ComponentShapeType)>());
    ShapeSettingsType["shapeType"] = &ShapeCreationSettings::shapeType;
    ShapeSettingsType["radius"] = &ShapeCreationSettings::radius;
    ShapeSettingsType["halfExtents"] = &ShapeCreationSettings::halfExtents;
    ShapeSettingsType["halfCylinderHeight"] = &ShapeCreationSettings::halfCylinderHeight;

    // PhysicsComponentSettings
    sol::usertype<PhysicsComponentSettings> PhysicsComponentSettingsType =
        luaState.new_usertype<PhysicsComponentSettings>(
            "PhysicsComponentSettings", sol::constructors<PhysicsComponentSettings(Entity, ComponentShapeType)>());
    PhysicsComponentSettingsType["entity"] = &PhysicsComponentSettings::entity;
    PhysicsComponentSettingsType["activate"] = &PhysicsComponentSettings::activate;
    PhysicsComponentSettingsType["motionType"] = &PhysicsComponentSettings::motionType;
    PhysicsComponentSettingsType["layer"] = &PhysicsComponentSettings::layer;
    PhysicsComponentSettingsType["isTrigger"] = &PhysicsComponentSettings::isTrigger;
    PhysicsComponentSettingsType["gravityFactor"] = &PhysicsComponentSettings::gravityFactor;
    PhysicsComponentSettingsType["shapeSettings"] = &PhysicsComponentSettings::shapeSettings;

    // Register ShapeRef wrapper for shapes returned by BodyGetter
    sol::usertype<ShapeRef> ShapeRefType = luaState.new_usertype<ShapeRef>("ShapeRef", sol::constructors<ShapeRef()>());
    ShapeRefType.set_function("IsValid", &ShapeRef::IsValid);
    ShapeRefType.set_function("Get", &ShapeRef::Get);

    // Physics table
    sol::table physicsTable = luaState.create_table();

    // Provide Lua-side BodyGetter proxy: returns a table with methods that create a C++ BodyGetter and call methods
    physicsTable.set_function("GetBodyGetter",
                              [&luaState](Entity e)
                              {
                                  sol::state_view lua(luaState);
                                  sol::table tbl = lua.create_table();

                                  tbl.set_function("GetPosition",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetPosition();
                                                   });
                                  tbl.set_function("GetRotation",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetRotation();
                                                   });
                                  tbl.set_function("GetShape",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return ShapeRef(g.GetShape());
                                                   });
                                  tbl.set_function("GetShapeType",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetShapeType();
                                                   });
                                  tbl.set_function("GetVelocity",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetVelocity();
                                                   });
                                  tbl.set_function("GetMaxLinearVelocity",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetMaxLinearVelocity();
                                                   });
                                  tbl.set_function("GetAngularVelocity",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetAngularVelocity();
                                                   });
                                  tbl.set_function("GetMaxAngularVelocity",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetMaxAngularVelocity();
                                                   });
                                  tbl.set_function("GetGravityFactor",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetGravityFactor();
                                                   });
                                  tbl.set_function("IsTrigger",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.isTrigger();
                                                   });
                                  tbl.set_function("GetObjectLayer",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetObjectLayer();
                                                   });
                                  tbl.set_function("GetMotionType",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetMotionType();
                                                   });
                                  tbl.set_function("GetSphereShapeRadius",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetSphereShapeRadius().value_or(0.0f);
                                                   });
                                  tbl.set_function("GetBoxShapeHalfExtents",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetBoxShapeHalfExtents().value_or(Vec3(0.0f));
                                                   });
                                  tbl.set_function("GetCylinderShapeHalfHeightAndRadius",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetCylinderShapeHalfHeightAndRadius();
                                                   });
                                  tbl.set_function("GetCapsuleShapeHalfHeightAndRadius",
                                                   [e]()
                                                   {
                                                       auto g = Blainn::PhysicsSubsystem::GetBodyGetter(e);
                                                       return g.GetCapsuleShapeHalfHeightAndRadius();
                                                   });

                                  return tbl;
                              });

    // Provide Lua-side BodyUpdater proxy: returns a table with methods that create a C++ BodyUpdater and call methods
    physicsTable.set_function("GetBodyUpdater",
                              [&luaState](Entity e)
                              {
                                  sol::state_view lua(luaState);
                                  sol::table tbl = lua.create_table();

                                  tbl.set_function("SetPosition",
                                                   [e](const Vec3 &p, int activationInt)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetPosition(p, static_cast<EActivation>(activationInt));
                                                   });
                                  tbl.set_function("SetRotation",
                                                   [e](const Quat &r, int activationInt)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetRotation(r, static_cast<EActivation>(activationInt));
                                                   });
                                  tbl.set_function("SetScale",
                                                   [e](const Vec3 &s, const Vec3 &prev)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetScale(s, prev);
                                                   });
                                  tbl.set_function("SetVelocity",
                                                   [e](const Vec3 &v)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetVelocity(v);
                                                   });
                                  tbl.set_function("SetMaxLinearVelocity",
                                                   [e](float v)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetMaxLinearVelocity(v);
                                                   });
                                  tbl.set_function("SetAngularVelocity",
                                                   [e](const Vec3 &v)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetAngularVelocity(v);
                                                   });
                                  tbl.set_function("SetMaxAngularVelocity",
                                                   [e](float v)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetMaxAngularVelocity(v);
                                                   });
                                  tbl.set_function("SetGravityFactor",
                                                   [e](float g)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetGravityFactor(g);
                                                   });
                                  tbl.set_function("SetObjectLayer",
                                                   [e](int layer)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetObjectLayer(static_cast<JPH::ObjectLayer>(layer));
                                                   });
                                  tbl.set_function("AddVelocity",
                                                   [e](const Vec3 &d)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.AddVelocity(d);
                                                   });
                                  tbl.set_function("AddImpulse",
                                                   [e](const Vec3 &i)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.AddImpulse(i);
                                                   });
                                  tbl.set_function("AddAngularImpulse",
                                                   [e](const Vec3 &i)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.AddAngularImpulse(i);
                                                   });
                                  tbl.set_function("AddForce",
                                                   [e](const Vec3 &f)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.AddForce(f);
                                                   });
                                  tbl.set_function("ReplaceBodyShape",
                                                   [e](ShapeCreationSettings &s, int activationInt)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.ReplaceBodyShape(s, static_cast<EActivation>(activationInt));
                                                   });
                                  tbl.set_function("SetMotionType",
                                                   [e](int m, int activationInt)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetMotionType(static_cast<PhysicsComponentMotionType>(m),
                                                                       static_cast<EActivation>(activationInt));
                                                   });
                                  tbl.set_function("SetSphereShapeSettings",
                                                   [e](float radius)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetSphereShapeSettings(radius);
                                                   });
                                  tbl.set_function("SetBoxShapeSettings",
                                                   [e](const Vec3 &halfExtents)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetBoxShapeSettings(halfExtents);
                                                   });
                                  tbl.set_function("SetCapsuleShapeSettings",
                                                   [e](float halfHeight, float radius)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetCapsuleShapeSettings(halfHeight, radius);
                                                   });
                                  tbl.set_function("SetCylinderShapeSettings",
                                                   [e](float halfHeight, float radius)
                                                   {
                                                       auto u = Blainn::PhysicsSubsystem::GetBodyUpdater(e);
                                                       u.SetCylinderShapeSettings(halfHeight, radius);
                                                   });

                                  return tbl;
                              });


    physicsTable.set_function("CreateAttachPhysicsComponent",
                              sol::overload(
                                  [](Entity entity, int shapeTypeInt)
                                  {
                                      PhysicsComponentSettings s(entity, static_cast<ComponentShapeType>(shapeTypeInt));
                                      Blainn::PhysicsSubsystem::CreateAttachPhysicsComponent(s);
                                  },
                                  [](PhysicsComponentSettings &settings)
                                  { Blainn::PhysicsSubsystem::CreateAttachPhysicsComponent(settings); }));

    physicsTable.set_function("HasPhysicsComponent",
                              [](Entity e) { return Blainn::PhysicsSubsystem::HasPhysicsComponent(e); });
    physicsTable.set_function("DestroyPhysicsComponent",
                              [](Entity e) { Blainn::PhysicsSubsystem::DestroyPhysicsComponent(e); });

    physicsTable.set_function("IsBodyActive", [](Entity e) { return Blainn::PhysicsSubsystem::IsBodyActive(e); });
    physicsTable.set_function("ActivateBody", [](Entity e) { Blainn::PhysicsSubsystem::ActivateBody(e); });
    physicsTable.set_function("DeactivateBody", [](Entity e) { Blainn::PhysicsSubsystem::DeactivateBody(e); });

    // CastRay returns optional<RayCastResult> - convert to nil or table
    physicsTable.set_function("CastRay",
                              [&luaState](const Vec3 &origin, const Vec3 &dir)
                              {
                                  auto res = Blainn::PhysicsSubsystem::CastRay(origin, dir);
                                  sol::state_view lua(luaState);
                                  if (!res) return sol::object(sol::nil);
                                  return sol::make_object(lua, *res);
                              });

    // Event listener: wrap Lua function and call with a PhysicsEvent table
    auto add_listener_func = [&luaState](int eventTypeInt, sol::function listener) -> uint64_t
    {
        PhysicsEventType eventType = static_cast<PhysicsEventType>(eventTypeInt);
        sol::function luaListener = listener;
        uint64_t id = s_nextListenerId.fetch_add(1);

        auto handle = Blainn::PhysicsSubsystem::AddEventListener(
            eventType,
            [&luaState, luaListener](const eastl::shared_ptr<PhysicsEvent> &ev)
            {
                sol::state_view lua(luaState);
                sol::table tbl = lua.create_table();
                tbl["eventType"] = static_cast<int>(ev->eventType);
                tbl["entity1"] = ev->entity1.str();
                tbl["entity2"] = ev->entity2.str();

                sol::protected_function pfunc = luaListener;
                sol::protected_function_result result = pfunc(tbl);
                if (!result.valid())
                {
                    sol::error err = result;
                    BF_ERROR("Lua physics listener error: " + eastl::string(err.what()));
                }
            });

        s_listenerHandles[id] = handle;
        return id;
    };

    // TODO: забанить?
    physicsTable.set_function("AddEventListener", add_listener_func);

    // TODO: забанить?
    physicsTable.set_function("RemoveEventListener",
                              [](int eventTypeInt, uint64_t id)
                              {
                                  auto it = s_listenerHandles.find(id);
                                  if (it == s_listenerHandles.end()) return;
                                  PhysicsEventType eventType = static_cast<PhysicsEventType>(eventTypeInt);
                                  Blainn::PhysicsSubsystem::RemoveEventListener(eventType, it->second);
                                  s_listenerHandles.erase(it);
                              });

    luaState["Physics"] = physicsTable;
}

#endif
