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

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterPhysicsTypes(sol::state &luaState)
{
    luaState.new_enum<PhysicsEventType, true>("PhysicsEventType", {
                                                  {"CollisionStarted", PhysicsEventType::CollisionStarted},
                                                  {"CollisionEnded",   PhysicsEventType::CollisionEnded}
                                              });

    luaState.new_enum<ComponentShapeType, true>("ComponentShapeType", {
                                                    {"Sphere",   ComponentShapeType::Sphere},
                                                    {"Box",      ComponentShapeType::Box},
                                                    {"Capsule",  ComponentShapeType::Capsule},
                                                    {"Cylinder", ComponentShapeType::Cylinder},
                                                    {"Empty",    ComponentShapeType::Empty}
                                                });

    luaState.new_enum<PhysicsComponentMotionType, true>("PhysicsComponentMotionType", {
                                                             {"Static",    PhysicsComponentMotionType::Static},
                                                             {"Kinematic", PhysicsComponentMotionType::Kinematic},
                                                             {"Dynamic",   PhysicsComponentMotionType::Dynamic}
                                                         });

    luaState.new_enum<EActivation, true>("EActivation", {
                                             {"DontActivate", EActivation::DontActivate},
                                             {"Activate",     EActivation::Activate}
                                         });

    // Object layers
    luaState.new_enum<ObjectLayer, true>("ObjectLayer", {
                                             {"UNUSED1",    Layers::UNUSED1},
                                             {"UNUSED2",    Layers::UNUSED2},
                                             {"UNUSED3",    Layers::UNUSED3},
                                             {"UNUSED4",    Layers::UNUSED4},
                                             {"NON_MOVING", Layers::NON_MOVING},
                                             {"MOVING",     Layers::MOVING},
                                             {"DEBRIS",     Layers::DEBRIS},
                                             {"SENSOR",     Layers::SENSOR}
                                         });

    // Broad phase layers (use integer values matching BroadPhaseLayers)
    luaState.new_enum<int, true>("BroadPhaseLayer", {
                                     {"NON_MOVING", 0},
                                     {"MOVING",     1},
                                     {"DEBRIS",     2},
                                     {"SENSOR",     3},
                                     {"UNUSED",     4}
                                 });

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

    physicsTable.set_function("IsBodyActive",   [](Entity e) { return PhysicsSubsystem::IsBodyActive(e); });
    physicsTable.set_function("ActivateBody",   [](Entity e) { PhysicsSubsystem::ActivateBody(e); });
    physicsTable.set_function("DeactivateBody", [](Entity e) { PhysicsSubsystem::DeactivateBody(e); });

    auto table_to_layers = [](const sol::table &tbl)
    {
        eastl::vector<ObjectLayer> layers;
        for (auto &kv : tbl)
        {
            sol::object o = kv.second;
            if (o.is<int>())
            {
                layers.push_back(static_cast<ObjectLayer>(o.as<int>()));
            }
            else if (o.is<unsigned int>())
            {
                layers.push_back(static_cast<ObjectLayer>(o.as<unsigned int>()));
            }
            else if (o.is<sol::object>())
            {
                // fallback try as ObjectLayer directly
                try
                {
                    layers.push_back(o.as<ObjectLayer>());
                }
                catch (const std::exception &ex)
                {
                    BF_WARN("Failed to convert Lua layer to ObjectLayer: {}", ex.what());
                }
            }
        }
        return layers;
    };

    auto table_to_entity_queue = [](const sol::table &tbl)
    {
        eastl::queue<uuid> entities;
        for (auto &kv : tbl)
        {
            sol::object o = kv.second;
            if (o.is<std::string>())
            {
                uuid id(o.as<std::string>());
                entities.push(id);
            }
        }
        return entities;
    };

    physicsTable.set_function(
        "CastRay",
        sol::overload(
            // origin, dir (Vec3)
            [&luaState](const Vec3 &origin, const Vec3 &dir)
            {
                eastl::queue<uuid> entities;
                eastl::vector<ObjectLayer> layers;
                auto res = PhysicsSubsystem::FilteredCastRay(entities, origin, dir, layers);
                sol::state_view lua(luaState);
                if (!res) return sol::object(sol::nil);
                sol::table t = lua.create_table();
                RayCastResult rayCastResult = res.value();
                t["entityId"] = rayCastResult.entityId.str();
                t["distance"] = rayCastResult.distance;
                t["hitNormal"] = rayCastResult.hitNormal;
                t["hitPoint"] = rayCastResult.hitPoint;
                return sol::object(t);
            },
            // origin, dir, ignoredLayers (Vec3 origin/dir)
            [&luaState, table_to_layers](const Vec3 &origin, const Vec3 &dir, sol::table ignoredLayersTbl)
            {
                eastl::queue<uuid> entities;
                auto layers = table_to_layers(ignoredLayersTbl);
                auto res = PhysicsSubsystem::FilteredCastRay(entities, origin, dir, layers);
                sol::state_view lua(luaState);
                if (!res) return sol::object(sol::nil);
                sol::table t = lua.create_table();
                RayCastResult rayCastResult = res.value();
                t["entityId"] = rayCastResult.entityId.str();
                t["distance"] = rayCastResult.distance;
                t["hitNormal"] = rayCastResult.hitNormal;
                t["hitPoint"] = rayCastResult.hitPoint;
                return sol::object(t);
            },
            // origin, dir, ignoredLayers, ignoredEntityIds (Vec3 origin/dir)
            // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            [&luaState, table_to_layers, table_to_entity_queue](const Vec3 &origin, const Vec3 &dir, sol::table ignoredLayersTbl, sol::table ignoredEntityIdsTbl)
            {
                auto entities = table_to_entity_queue(ignoredEntityIdsTbl);
                auto layers = table_to_layers(ignoredLayersTbl);
                auto res = PhysicsSubsystem::FilteredCastRay(entities, origin, dir, layers);
                sol::state_view lua(luaState);
                if (!res) return sol::object(sol::nil);
                sol::table t = lua.create_table();
                RayCastResult rayCastResult = res.value();
                t["entityId"] = rayCastResult.entityId.str();
                t["distance"] = rayCastResult.distance;
                t["hitNormal"] = rayCastResult.hitNormal;
                t["hitPoint"] = rayCastResult.hitPoint;
                return sol::object(t);
            }));

    luaState["Physics"] = physicsTable;
}

#endif
