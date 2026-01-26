#include "pch.h"

#include "scripting/LuaScript.h"

#include "subsystems/ScriptingSubsystem.h"
#include "subsystems/PhysicsSubsystem.h"


using namespace Blainn;

LuaScript::LuaScript()
{
    m_id = Rand::getRandomUUID();
}

Blainn::LuaScript::~LuaScript()
{
    RemovePhysicsEventListeners();
}

bool LuaScript::Load(const Path &scriptPath, const Entity &owningEntity)
{
    m_scriptPath = scriptPath;

    sol::state &lua = ScriptingSubsystem::GetLuaState();

    m_environment = sol::environment(lua, sol::create, lua.globals());
    m_owningEntityId = owningEntity.GetUUID();
    m_environment["OwningEntity"] = m_owningEntityId.bytes();

    sol::load_result script = lua.load_file(scriptPath.string());
    if (!script.valid())
    {
        sol::error err = script;
        BF_ERROR("Failed to load Lua script: {}\nError: {}", m_scriptPath.string(), err.what());
        return false;
    }

    sol::protected_function scriptAsFunc = script.get<sol::protected_function>();

    sol::set_environment(m_environment, scriptAsFunc);
    // load lua functions to environment
    sol::protected_function_result result = script();
    if (!result.valid())
    {
        sol::error err = result;
        BF_ERROR("Failed to execute Lua script: {}\nError: {}", m_scriptPath.string(), err.what());
        return false;
    }

    if (HasFunction(PredefinedFunctions::kOnStart)) m_predefinedFunctions.insert(PredefinedFunctions::kOnStart);
    if (HasFunction(PredefinedFunctions::kOnUpdate)) m_predefinedFunctions.insert(PredefinedFunctions::kOnUpdate);
    if (HasFunction(PredefinedFunctions::kOnDestroy)) m_predefinedFunctions.insert(PredefinedFunctions::kOnDestroy);

    if (HasFunction(PredefinedFunctions::kOnCollisionStarted))
    {
        m_predefinedFunctions.insert(PredefinedFunctions::kOnCollisionStarted);
        m_onCollisionStartedHandle = PhysicsSubsystem::AddEventListener(
            PhysicsEventType::CollisionStarted,
            [this](const eastl::shared_ptr<PhysicsEvent> &physicsEvent) { OnCollisionStartedCall(physicsEvent); });
    }
    if (HasFunction(PredefinedFunctions::kOnCollisionEnded))
    {
        m_predefinedFunctions.insert(PredefinedFunctions::kOnCollisionEnded);
        m_onCollisionEndedHandle = PhysicsSubsystem::AddEventListener(
            PhysicsEventType::CollisionEnded,
            [this](const eastl::shared_ptr<PhysicsEvent> &physicsEvent) { OnCollisionEndedCall(physicsEvent); });
    }

    if (HasFunction(PredefinedFunctions::kOnDrawUI)) m_predefinedFunctions.insert(PredefinedFunctions::kOnDrawUI);

    m_isLoaded = true;
    return true;
}

bool LuaScript::IsLoaded() const
{
    return m_isLoaded;
}

const Path &LuaScript::GetScriptPath() const
{
    return m_scriptPath;
}

const uuid &Blainn::LuaScript::GetId() const
{
    return m_id;
}

const sol::table &Blainn::LuaScript::GetEnvironment() const
{
    return m_environment;
}

void Blainn::LuaScript::SetEnvVar(const eastl::string &name, const sol::object &var)
{
    m_environment[name.c_str()] = var;
}

bool Blainn::LuaScript::HasFunction(const eastl::string &functionName) const
{
    sol::protected_function customFunc = m_environment[functionName.data()];
    if (!customFunc.valid())
    {
        return false;
    }
    return true;
}

bool LuaScript::OnStartCall()
{
    if (!m_predefinedFunctions.contains(PredefinedFunctions::kOnStart)) return false;
    return CustomCall(PredefinedFunctions::kOnStart);
}

bool LuaScript::OnUpdateCall(float deltaTimeMs)
{
    if (!m_predefinedFunctions.contains(PredefinedFunctions::kOnUpdate)) return false;
    return CustomCall(PredefinedFunctions::kOnUpdate, deltaTimeMs);
}

bool LuaScript::OnDestroyCall()
{
    RemovePhysicsEventListeners();
    if (!m_predefinedFunctions.contains(PredefinedFunctions::kOnDestroy)) return false;
    return CustomCall(PredefinedFunctions::kOnDestroy);
}

bool Blainn::LuaScript::OnCollisionStartedCall(const eastl::shared_ptr<PhysicsEvent> &physicsEvent)
{
    if (physicsEvent->entity1 != m_owningEntityId && physicsEvent->entity2 != m_owningEntityId) return false;
    if (!m_predefinedFunctions.contains(PredefinedFunctions::kOnCollisionStarted)) return false;
    sol::state &lua = ScriptingSubsystem::GetLuaState();
    sol::state_view sv(lua);
    sol::table tbl = sv.create_table();
    tbl["eventType"] = static_cast<int>(physicsEvent->eventType);
    tbl["entity1"] = physicsEvent->entity1.str();
    tbl["entity2"] = physicsEvent->entity2.str();
    return CustomCall(PredefinedFunctions::kOnCollisionStarted, tbl);
}

bool Blainn::LuaScript::OnCollisionEndedCall(const eastl::shared_ptr<PhysicsEvent> &physicsEvent)
{
    if (physicsEvent->entity1 != m_owningEntityId && physicsEvent->entity2 != m_owningEntityId) return false;
    if (!m_predefinedFunctions.contains(PredefinedFunctions::kOnCollisionEnded)) return false;
    sol::state &lua = ScriptingSubsystem::GetLuaState();
    sol::state_view sv(lua);
    sol::table tbl = sv.create_table();
    tbl["eventType"] = static_cast<int>(physicsEvent->eventType);
    tbl["entity1"] = physicsEvent->entity1.str();
    tbl["entity2"] = physicsEvent->entity2.str();
    return CustomCall(PredefinedFunctions::kOnCollisionEnded, tbl);
}

void Blainn::LuaScript::RemovePhysicsEventListeners()
{
    if (m_predefinedFunctions.contains(PredefinedFunctions::kOnCollisionStarted))
    {
        PhysicsSubsystem::RemoveEventListener(PhysicsEventType::CollisionStarted, m_onCollisionStartedHandle);
    }

    if (m_predefinedFunctions.contains(PredefinedFunctions::kOnCollisionEnded))
    {
        PhysicsSubsystem::RemoveEventListener(PhysicsEventType::CollisionEnded, m_onCollisionEndedHandle);
    }
}

bool LuaScript::OnDrawUI()
{
    if (!m_predefinedFunctions.contains(PredefinedFunctions::kOnDrawUI)) return false;
    return CustomCall(PredefinedFunctions::kOnDrawUI);
}
