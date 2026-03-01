#pragma once

#include "aliases.h"

#include "components/ScriptingComponent.h"
#include "scene/Entity.h"
#include <EASTL/optional.h>

namespace sol
{
class state;
}

namespace Blainn
{

class ScriptingSubsystem
{
public:
    static void Init();
    static void Destroy();

    static void LoadAllScripts(Scene &scene);
    static void UnloadAllScripts(Scene &scene);

    static void Update(Scene &scene, float deltaTimeMs);

    static void CreateAttachScriptingComponent(Entity entity);
    static void DestroyScriptingComponent(Entity entity);

    static sol::state &GetLuaState();

    static sol::object GetValueFromScript(const uuid &scriptUuid, const eastl::string &valueName);
    static void SetValueInScript(const uuid &scriptUuid, const eastl::string &valueName, const sol::object &value);

    /// @brief Load Lua Script
    /// @param path - script path relative to content folder
    /// @param callOnStart - call OnStart() script function. true by default
    /// @return returns loaded script uuid
    static eastl::optional<uuid> LoadScript(Entity entity, const Path &path, bool callOnStart = true);

    /// @brief Unload Lua Script. OnDestroy() script function called automatically
    static void UnloadScript(const uuid &scriptUuid);

    /// @brief Load AI script with specific structure
    /// @param path - ai script path relative to content folder
    static eastl::unique_ptr<LuaScript> LoadAiScript(Entity entity, const Path &path);

    template <typename... Args>
    static bool CallScriptFunction(const uuid &scriptUuid, const eastl::string &functionName, Args... args)
    {
        if (!m_scriptEntityConnections.contains(scriptUuid))
        {
            BF_ERROR("Script " + scriptUuid.str() + " function call error - no script");
            return false;
        }

        ScriptingComponent *component = m_scriptEntityConnections.at(scriptUuid).TryGetComponent<ScriptingComponent>();
        if (!component)
        {
            BF_ERROR("Script" + scriptUuid.str() + " function call error - component not exist");
            return false;
        }

        if (!component->scripts.contains(scriptUuid))
        {
            BF_ERROR("Script" + scriptUuid.str() + " not contained in component");
            return false;
        }

        return component->scripts.at(scriptUuid).CustomCall(functionName, std::forward<Args>(args)...);
    }

    static uuid GetEntityUUIDByScriptUUID(const uuid &scriptUuid)
    {
        return m_scriptEntityConnections[scriptUuid].GetUUID();
    }

private:
    ScriptingSubsystem() = delete;

    inline static bool m_isInitialized = false;
    inline static sol::state m_lua = sol::state();

    inline static eastl::unordered_map<uuid, Entity> m_scriptEntityConnections = eastl::unordered_map<uuid, Entity>{};

    static void RegisterBlainnTypes();

#ifdef BLAINN_TEST_LUA_SCRIPTS
    inline static Entity m_scriptTestEntity{};
    inline static uuid m_scriptTestUuid1{};
    inline static uuid m_scriptTestUuid2{};
#endif
};
} // namespace Blainn
