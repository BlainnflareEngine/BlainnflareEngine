#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"
#include "components/AIControllerComponent.h"
#include "ai/AIController.h"
#include "subsystems/Navigation/NavigationSubsystem.h"
#include "Engine.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

void Blainn::RegisterNavigationTypes(sol::state &luaState)
{
    sol::table navigationTable = luaState.create_table();

    navigationTable.set_function("IsNavMeshLoaded", []() -> bool { return NavigationSubsystem::IsNavMeshLoaded(); });

    navigationTable.set_function("LoadNavMesh", [](const std::string &relativePath) -> bool
                          { return NavigationSubsystem::LoadNavMesh(relativePath); });

    navigationTable.set_function("ClearNavMesh", []() { NavigationSubsystem::ClearNavMesh(); });

    navigationTable.set_function("SetNavMeshDebugDraw",
                          [](bool enabled) { NavigationSubsystem::SetShouldDrawDebug(enabled); });

    navigationTable.set_function("GetNavMeshDebugDraw",
                                 []() -> bool { return NavigationSubsystem::ShouldDrawDebug(); });

    navigationTable.set_function("FindRandomPointOnNavMeshInRadius",
                                 [](Vec3 &outPoint, const Vec3 &center, float radius) -> std::pair<bool, Vec3>
                                 { return NavigationSubsystem::FindRandomPointOnNavMesh(center, radius); });

    navigationTable.set_function("FindRandomPointOnNavMesh",
                                 [](Vec3 &outPoint) -> std::pair<bool, Vec3>
                                 { return NavigationSubsystem::FindRandomPointOnNavMesh(); });

    luaState["Navigation"] = navigationTable;

    auto AIControllerType = luaState.new_usertype<AIController>("AIController", sol::no_constructor);

    luaState.set_function("GetAIController",
                          [](const std::string &idStr) -> AIController *
                          {
                              uuid entityId = uuid(idStr);
                              Scene *scene = Engine::GetActiveScene().get();
                              if (!scene)
                              {
                                  return nullptr;
                              }

                              Entity entity = scene->GetEntityWithUUID(entityId);
                              if (!entity.IsValid())
                              {
                                  return nullptr;
                              }

                              AIControllerComponent *aiComp = entity.TryGetComponent<AIControllerComponent>();
                              if (!aiComp)
                              {
                                  return nullptr;
                              }

                              return &aiComp->aiController;
                          });

    AIControllerType.set_function("Possess",
                                  [](AIController *controller, const std::string &idStr) -> bool
                                  {
                                      uuid entityId = uuid(idStr);
                                      Scene *scene = Engine::GetActiveScene().get();
                                      if (!scene)
                                      {
                                          return false;
                                      }

                                      Entity entity = scene->GetEntityWithUUID(entityId);
                                      if (!entity.IsValid())
                                      {
                                          return false;
                                      }

                                      if (!controller)
                                      {
                                          return false;
                                      }

                                      controller->Possess(entity);
                                      return true;
                                  });

    AIControllerType.set_function("MoveTo",
                                  [](AIController *controller, const Vec3 &target) -> bool
                                  {
                                      if (!controller)
                                      {
                                          return false;
                                      }
                                      return controller->MoveTo(target);
                                  });

    AIControllerType.set_function("StopMoving",
                                  [](AIController *controller)
                                  {
                                      if (controller)
                                      {
                                          controller->StopMoving();
                                      }
                                  });

    AIControllerType.set_function("StartMoving",
                                  [](AIController *controller)
                                  {
                                      if (controller)
                                      {
                                          controller->StartMoving();
                                      }
                                  });

    AIControllerType.set_function("IsMoving",
                                  [](AIController *controller) -> bool
                                  {
                                      if (!controller)
                                      {
                                          return false;
                                      }
                                      Vec3 dummy;
                                      return controller->IsMoving();
                                  });

    AIControllerType.set_function("GetBlackboard",
                                  [](AIController *controller) -> Blackboard *
                                  {
                                      if (!controller)
                                      {
                                          return nullptr;
                                      }
                                      return &controller->GetBlackboard();
                                  });
}
#endif