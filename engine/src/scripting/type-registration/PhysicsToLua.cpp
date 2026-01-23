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

#include <atomic>
#include <unordered_map>

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

// Listener handle storage for Lua -> PhysicsSubsystem event listeners
static inline std::unordered_map<uint64_t, PhysicsEventHandle> s_listenerHandles;
static inline std::atomic<uint64_t> s_nextListenerId{1};

void Blainn::RegisterPhysicsTypes(sol::state &luaState)
{
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

    // RayCastResult
    sol::usertype<RayCastResult> RayCastResultType = luaState.new_usertype<RayCastResult>("RayCastResult");
    RayCastResultType["entityId"] = &RayCastResult::entityId;
    RayCastResultType["hitPoint"] = &RayCastResult::hitPoint;
    RayCastResultType["hitNormal"] = &RayCastResult::hitNormal;
    RayCastResultType["distance"] = &RayCastResult::distance;

    sol::table physicsTable = luaState.create_table();

    RegisterPhysicsBodyGetter(luaState, physicsTable);
    RegisterPhysicsBodyUpdater(luaState, physicsTable);

    physicsTable.set_function("CreateAttachPhysicsComponent",
                              sol::overload(
                                  [](Entity entity, int shapeTypeInt)
                                  {
                                      PhysicsComponentSettings s(entity, static_cast<ComponentShapeType>(shapeTypeInt));
                                      PhysicsSubsystem::CreateAttachPhysicsComponent(s);
                                  },
                                  [](PhysicsComponentSettings &settings)
                                  { PhysicsSubsystem::CreateAttachPhysicsComponent(settings); }));

    physicsTable.set_function("HasPhysicsComponent", [](Entity e) { return PhysicsSubsystem::HasPhysicsComponent(e); });
    physicsTable.set_function("DestroyPhysicsComponent",
                              [](Entity e) { PhysicsSubsystem::DestroyPhysicsComponent(e); });

    physicsTable.set_function("IsBodyActive", [](Entity e) { return PhysicsSubsystem::IsBodyActive(e); });
    physicsTable.set_function("ActivateBody", [](Entity e) { PhysicsSubsystem::ActivateBody(e); });
    physicsTable.set_function("DeactivateBody", [](Entity e) { PhysicsSubsystem::DeactivateBody(e); });

    physicsTable.set_function("CastRay",
                              [&luaState](const Vec3 &origin, const Vec3 &dir)
                              {
                                  auto res = PhysicsSubsystem::CastRay(origin, dir);
                                  sol::state_view lua(luaState);
                                  if (!res) return sol::object(sol::nil);
                                  sol::table t = luaState.create_table();
                                  RayCastResult rayCastResult = res.value();
                                  t["entityId"] = rayCastResult.entityId.str();
                                  t["distance"] = rayCastResult.distance;
                                  t["hitNormal"] = rayCastResult.hitNormal;
                                  t["hitPoint"] = rayCastResult.hitPoint;
                                  return sol::object(t);
                              });

    luaState["Physics"] = physicsTable;
}

#endif
