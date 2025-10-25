#pragma once

#include <EASTL/optional.h>
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>
#include <sol/sol.hpp>

#include "aliases.h"
#include "runtime/components/ScriptingComponent.h"
#include "scene/Scene.h"

namespace Blainn
{
class ScriptingSubsystem
{
public:
    static void Init();
    static void Destroy();

    static void Update(float deltaTimeMs);

    static sol::state &GetLuaState();

    /// @param path - ralative to cwd or absolute path
    static void SetLuaScriptsFolder(const eastl::string &path);

    /// @param path - script path in scripts content folder
    /// @param callOnStart - call OnStart() script function. true by default
    /// @return returns loaded script uuid
    static eastl::optional<uuid> LoadScript(Entity entity, const eastl::string &path, bool callOnStart = true);

    /// @brief OnDestroy() script function called automatically
    static void UnloadScript(const uuid &scriptUuid);

    template <typename... Args>
    static bool CallScriptFunction(const uuid &scriptUuid, const eastl::string &functionName, Args... args)
    {
        if (!m_scriptEntityConnections.contains(scriptUuid))
        {
            BF_ERROR("Script " + scriptUuid.str() + " function call error - no script");
            return;
        }

        ScriptingComponent *component = m_scriptEntityConnections.at(scriptUuid).TryGetComponent<ScriptingComponent>();
        if (!component)
        {
            BF_ERROR("Script" + scriptUuid.str() + " function call error - component not exist");
            return;
        }

        if (!component->scripts.contains(scriptUuid))
        {
            BF_ERROR("Script" + scriptUuid.str() + " not contained in component");
            return;
        }

        return component->scripts.at(scriptUuid).CustomCall(functionName, std::forward<Args>(args)...);
    }

private:
    ScriptingSubsystem() = delete;
    ScriptingSubsystem(const ScriptingSubsystem &) = delete;
    ScriptingSubsystem &operator=(const ScriptingSubsystem &) = delete;
    ScriptingSubsystem(const ScriptingSubsystem &&) = delete;
    ScriptingSubsystem &operator=(const ScriptingSubsystem &&) = delete;

    inline static bool m_isInitialized = false;
    inline static sol::state m_lua = sol::state();

    inline static eastl::string m_luaScriptsFolder = "content/scripts/";

    inline static eastl::unordered_map<uuid, Entity> m_scriptEntityConnections = eastl::unordered_map<uuid, Entity>{};
};
} // namespace Blainn