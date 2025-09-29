//
// Created by gorev on 28.09.2025.
//

#pragma once
#include <filesystem>

struct SettingsData
{
    SettingsData();
    SettingsData(const std::filesystem::path &contentDir)
        : m_contentDirectory(contentDir)
    {
    }

    std::filesystem::path m_contentDirectory;
};