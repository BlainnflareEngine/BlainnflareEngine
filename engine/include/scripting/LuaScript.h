#pragma once

#include "helpers.h"
#include "aliases.h"

#include "physics/PhysicsEvents.h"

namespace Blainn
{
class Entity;

class LuaScript
{
public:
    LuaScript();
    ~LuaScript();

    bool Load(const Path &path, const Entity &owningEntity);
    bool IsLoaded() const;

    const Path &GetScriptPath() const;
    const uuid &GetId() const;
    const sol::table &GetEnvironment() const;
    void SetEnvVar(const eastl::string &name, const sol::object &var);

    bool HasFunction(const eastl::string &functionName) const;
    bool OnStartCall();
    bool OnUpdateCall(float deltaTimeMs);
    bool OnDestroyCall();
    bool OnCollisionStartedCall(const eastl::shared_ptr<PhysicsEvent> &physicsEvent);
    bool OnCollisionEndedCall(const eastl::shared_ptr<PhysicsEvent> &physicsEvent);

    template <typename... Args> bool CustomCall(eastl::string_view functionName = "", Args &&...args)
    {
        if (!m_isLoaded)
        {
            BF_ERROR("LuaScript::CustomCall: Script not loaded: {}", m_scriptPath.string().c_str());
            return false;
        }

        sol::protected_function customFunc = m_environment[functionName.data()];
        if (!customFunc.valid())
        {
            BF_ERROR("LuaScript::CustomCall: Function {} not found in script {}", functionName.data(),
                     m_scriptPath.string().c_str());
            return false;
        }
        sol::set_environment(m_environment, customFunc);
        sol::protected_function_result result = customFunc(std::forward<Args>(args)...);
        if (!result.valid())
        {
            sol::error err = result;
            BF_ERROR("LuaScript::CustomCall: Error calling function {} in script {} \nError: {}", functionName.data(),
                     m_scriptPath.string().c_str(), err.what());
            return false;
        }

        return true;
    }

private:
    bool m_isLoaded = false;
    uuid m_id;
    Path m_scriptPath; // stores absolute path
    sol::environment m_environment;

    struct PredefinedFunctions
    {
        inline const static eastl::string kOnStart = "OnStart";
        inline const static eastl::string kOnUpdate = "OnUpdate";
        inline const static eastl::string kOnDestroy = "OnDestroy";
        inline const static eastl::string kOnCollisionStarted = "OnCollisionStarted";
        inline const static eastl::string kOnCollisionEnded = "OnCollisionEnded";
    };
    eastl::unordered_set<eastl::string> m_predefinedFunctions;

    PhysicsEventHandle m_onCollisionStartedHandle;
    PhysicsEventHandle m_onCollisionEndedHandle;

    void RemovePhysicsEventListeners();
};
} // namespace Blainn