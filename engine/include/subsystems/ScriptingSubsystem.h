#pragma once

#include <sol/sol.hpp>

#include <entt/entt.hpp>

#include "aliases.h"

namespace Blainn
{
class ScriptingSubsystem
{
public:
    static void Init();
    static void Destroy();

    static void Update(float deltaTimeMs);

    static sol::state &GetLuaState();

    /// @brief
    /// @param path - ralative to cwd or absolute path
    static void SetLuaScriptsFolder(const eastl::string &path);

    /// @brief
    /// @param componentUuid - component uuid to attach to
    /// @param path - script path in scripts content folder
    /// @param callOnStart - call OnStart() script function. true by default
    /// @return returns loaded script uuid
    static uuid LoadScript(const uuid &componentUuid, const eastl::string &path, bool callOnStart = true);

    /// @brief OnDestroy() script function called automatically
    /// @param path - script path in scripts content folder
    static void UnloadScript(const uuid &scriptUuid, const eastl::string &path);

    /// @brief if function does not exist does nothing
    static bool TryCallScriptFunction(const eastl::string &functionName);
    /// @brief if function does not exist logs error
    static bool CallScriptFunction(const eastl::string &functionName);

    // TODO: replace entt entity with internal type
    static uuid CreateAttachComponent(entt::entity entityId);

    static void DestroyComponent(const uuid &componentUuid);
    // TODO: replace entt entity with internal type
    static void DestroyComponents(entt::entity entityId);

private:
    ScriptingSubsystem() = delete;
    ScriptingSubsystem(const ScriptingSubsystem &) = delete;
    ScriptingSubsystem &operator=(const ScriptingSubsystem &) = delete;
    ScriptingSubsystem(const ScriptingSubsystem &&) = delete;
    ScriptingSubsystem &operator=(const ScriptingSubsystem &&) = delete;

    inline static bool m_isInitialized = false;

    inline static sol::state lua = sol::state();

    eastl::string m_luaScriptsFolder = "content/scripts/";
};
} // namespace Blainn