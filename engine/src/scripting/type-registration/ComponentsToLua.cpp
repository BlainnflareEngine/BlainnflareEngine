#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"
#include "subsystems/ScriptingSubsystem.h"

#include "components/CameraComponent.h"
#include "components/MeshComponent.h"
#include "components/ScriptingComponent.h"
#include "handles/Handle.h"
#include "scene/BasicComponents.h"
#include "scene/TransformComponent.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterComponentTypes(sol::state &luaState)
{
    // Basic components
    sol::usertype<IDComponent> IDComponentType =
        luaState.new_usertype<IDComponent>("IDComponent", sol::constructors<IDComponent()>());
    IDComponentType["ID"] = sol::property([](const IDComponent &idc) { return idc.ID.str(); });

    sol::usertype<TagComponent> TagComponentType = luaState.new_usertype<TagComponent>(
        "TagComponent", sol::constructors<TagComponent(), TagComponent(const eastl::string &)>());
    TagComponentType["Tag"] =
        sol::property([](const TagComponent &t) { return std::string(t.Tag.c_str()); },
                      [](TagComponent &t, const std::string &s) { t.Tag = eastl::string(s.c_str()); });

    sol::usertype<RelationshipComponent> RelationshipComponentType = luaState.new_usertype<RelationshipComponent>(
        "RelationshipComponent", sol::constructors<RelationshipComponent()>());
    RelationshipComponentType["ParentHandle"] = sol::property(
        [](const RelationshipComponent &r) { return r.ParentHandle.str(); },
        [](RelationshipComponent &r, const std::string &id) { r.ParentHandle = uuid::fromStrFactory(id); });
    RelationshipComponentType.set_function("Children",
                                           [&luaState](RelationshipComponent &r)
                                           {
                                               sol::table tbl = luaState.create_table();
                                               int i = 1;
                                               for (const auto &c : r.Children)
                                                   tbl[i++] = c.str();
                                               return tbl;
                                           });

    // TransformComponent
    sol::usertype<TransformComponent> TransformComponentType =
        luaState.new_usertype<TransformComponent>("TransformComponent", sol::constructors<TransformComponent()>());
    TransformComponentType.set_function("GetTranslation",   &TransformComponent::GetTranslation);
    TransformComponentType.set_function("SetTranslation",   &TransformComponent::SetTranslation);
    TransformComponentType.set_function("GetScale",         &TransformComponent::GetScale);
    TransformComponentType.set_function("SetScale",         &TransformComponent::SetScale);
    TransformComponentType.set_function("GetTransform",     &TransformComponent::GetTransform);
    TransformComponentType.set_function("SetTransform",     &TransformComponent::SetTransform);
    TransformComponentType.set_function("GetRotationEuler", &TransformComponent::GetRotationEuler);
    TransformComponentType.set_function("SetRotationEuler", &TransformComponent::SetRotationEuler);
    TransformComponentType.set_function("GetForwardVector", &TransformComponent::GetForwardVector);
    TransformComponentType.set_function("GetUpVector",      &TransformComponent::GetUpVector);
    TransformComponentType.set_function("GetRightVector",   &TransformComponent::GetRightVector);

    // MeshComponent
    sol::usertype<MeshComponent> MeshComponentType = luaState.new_usertype<MeshComponent>(
        "MeshComponent", sol::constructors<MeshComponent(const eastl::shared_ptr<MeshHandle> &),
                                           MeshComponent(eastl::shared_ptr<MeshHandle> &&)>());
    MeshComponentType.set_function("GetHandleIndex",
                                   [](MeshComponent &m) { return m.MeshHandle ? m.MeshHandle->GetIndex() : 0u; });

    // ScriptingComponent (expose a method to list attached scripts by UUID)
    sol::usertype<ScriptingComponent> ScriptingComponentType =
        luaState.new_usertype<ScriptingComponent>("ScriptingComponent", sol::constructors<ScriptingComponent()>());
    ScriptingComponentType.set_function("ListScripts",
                                        [&luaState](ScriptingComponent &s)
                                        {
                                            sol::table tbl = luaState.create_table();
                                            int idx = 1;
                                            for (auto &kv : s.scripts)
                                                tbl[idx++] = kv.first.str();
                                            return tbl;
                                        });

    sol::usertype<CameraComponent> cameraComponentType = luaState.new_usertype<CameraComponent>(
        "CameraComponent", sol::constructors<CameraComponent()>());

    cameraComponentType.set_function("CreateAttachCameraComponent",
        [](CameraComponent& cam, float fov, float nearZ, float farZ, int32_t priority) {
            CameraComponent c;
            c.camera.SetFovDegrees(fov);
            c.camera.SetNearZ(nearZ);
            c.camera.SetFarZ(farZ);
            c.CameraPriority = priority;

    });

    cameraComponentType.set_function("SetFOV", []()
    {

    });
    cameraComponentType.set_function("SetPriority", [](int32_t newPriority)
    {

    });
}
#endif