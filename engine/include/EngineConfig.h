//
// Created by gorev on 26.01.2026.
//

#pragma once
#include "Engine.h"
#include "aliases.h"
#include "EASTL/string.h"

namespace Blainn
{
class EngineConfig
{
public:
    EngineConfig()
    {

        if (!std::filesystem::exists(m_configPath / m_configName)) CreateDefaultConfig();

        YAML::Node config = YAML::LoadFile((m_configPath / m_configName).string());

        m_defaultScene = config["DefaultScene"].as<std::string>("Scene.scene").c_str();
    }

    void SetDefaultScene(const eastl::string &sceneName)
    {
        m_defaultScene = sceneName;
    }

    Path GetDefaultSceneName() const
    {
        return m_defaultScene.c_str();
    }

    void SaveConfig() const
    {
        YAML::Node config = YAML::LoadFile((m_configPath / m_configName).string());

        config["DefaultScene"] = m_defaultScene.c_str();

        const Path configFilePath = m_configPath / m_configName;
        std::ofstream fout(configFilePath.string());
        fout << config;
    }

private:
    eastl::string m_defaultScene = "Scene.scene";

    const Path m_configPath = std::filesystem::current_path() / "Config";
    const std::string m_configName = "EngineConfig.yaml";

    void CreateDefaultConfig()
    {
        std::filesystem::create_directories(m_configPath);

        std::ofstream ofs(m_configPath / m_configName);
        ofs << "";
        ofs.close();

        SaveConfig();
    }
};
} // namespace Blainn
