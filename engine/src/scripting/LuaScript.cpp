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
    m_environment["OwningEntityID"] = m_owningEntityId.str();
    m_environment["OwningEntity"] = owningEntity;

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

    m_onStart  = m_environment["OnStart"];
    m_onUpdate = m_environment["OnUpdate"];
    m_onDestroy = m_environment["OnDestroy"];
    m_onCollisionStarted = m_environment["OnCollisionStarted"];
    m_onCollisionEnded = m_environment["OnCollisionEnded"];
    m_onDrawUI = m_environment["OnDrawUI"];

    if (m_onStart.valid()) sol::set_environment(m_environment, m_onStart);
    if (m_onUpdate.valid()) sol::set_environment(m_environment, m_onUpdate);
    if (m_onDestroy.valid()) sol::set_environment(m_environment, m_onDestroy);

    if (m_onCollisionStarted.valid())
    {
        sol::set_environment(m_environment, m_onCollisionStarted);
        m_onCollisionStartedHandle = PhysicsSubsystem::AddEventListener(
            PhysicsEventType::CollisionStarted,
            [this](const eastl::shared_ptr<PhysicsEvent> &physicsEvent) { OnCollisionStartedCall(physicsEvent); });
    }
    if (m_onCollisionEnded.valid())
    {
        sol::set_environment(m_environment, m_onCollisionEnded);
        m_onCollisionEndedHandle = PhysicsSubsystem::AddEventListener(
            PhysicsEventType::CollisionEnded,
            [this](const eastl::shared_ptr<PhysicsEvent> &physicsEvent) { OnCollisionEndedCall(physicsEvent); });
    }

    if (m_onDrawUI.valid()) sol::set_environment(m_environment, m_onDrawUI);

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
    if (!m_onStart.valid()) return false;
    auto result = m_onStart();
    if (!result.valid())
    {
        sol::error err = result;
        BF_ERROR("Lua OnStart error in {}:\n\t\t{}", m_scriptPath.string(), err.what());
        return false;
    }
    return true;
}

bool LuaScript::OnUpdateCall(float deltaTimeMs)
{
    if (!m_onUpdate.valid()) return false;

    auto result = m_onUpdate(deltaTimeMs);
    if (!result.valid())
    {
        sol::error err = result;
        BF_ERROR("Lua OnUpdate error in {}:\n\t\t{}", m_scriptPath.string(), err.what());
        return false;
    }

    return true;
}

bool LuaScript::OnDestroyCall()
{
    RemovePhysicsEventListeners();

    if (!m_onDestroy.valid()) return false;

    auto result = m_onDestroy();
    if (!result.valid())
    {
        sol::error err = result;
        BF_ERROR("Lua OnDestroy error in {}:\n\t\t{}", m_scriptPath.string(), err.what());
        return false;
    }

    return true;
}

bool Blainn::LuaScript::OnCollisionStartedCall(const eastl::shared_ptr<PhysicsEvent> &physicsEvent)
{
    if (!m_onCollisionStarted.valid()) return false;
    if (physicsEvent->entity1 != m_owningEntityId && physicsEvent->entity2 != m_owningEntityId) return false;

    sol::state_view sv(ScriptingSubsystem::GetLuaState());
    sol::table tbl = sv.create_table();
    tbl["eventType"] = static_cast<int>(physicsEvent->eventType);
    tbl["entity1"] = physicsEvent->entity1.str();
    tbl["entity2"] = physicsEvent->entity2.str();

    auto result = m_onCollisionStarted(tbl);
    return result.valid();
}

bool Blainn::LuaScript::OnCollisionEndedCall(const eastl::shared_ptr<PhysicsEvent> &physicsEvent)
{
    if (!m_onCollisionEnded.valid()) return false;
    if (physicsEvent->entity1 != m_owningEntityId && physicsEvent->entity2 != m_owningEntityId) return false;

    sol::state_view sv(ScriptingSubsystem::GetLuaState());
    sol::table tbl = sv.create_table();
    tbl["eventType"] = static_cast<int>(physicsEvent->eventType);
    tbl["entity1"] = physicsEvent->entity1.str();
    tbl["entity2"] = physicsEvent->entity2.str();

    auto result = m_onCollisionEnded(tbl);
    return result.valid();
}

void Blainn::LuaScript::RemovePhysicsEventListeners()
{
    if (m_onCollisionStarted.valid())
    {
        PhysicsSubsystem::RemoveEventListener(PhysicsEventType::CollisionStarted, m_onCollisionStartedHandle);
    }

    if (m_onCollisionEnded.valid())
    {
        PhysicsSubsystem::RemoveEventListener(PhysicsEventType::CollisionEnded, m_onCollisionEndedHandle);
    }
}

bool LuaScript::OnDrawUI()
{
    if (!m_onDrawUI.valid()) return false;

    auto result = m_onDrawUI();
    if (!result.valid())
    {
        sol::error err = result;
        BF_ERROR("Lua OnDrawUI error in {}:\n\t\t{}", m_scriptPath.string(), err.what());
        return false;
    }

    return true;
}
