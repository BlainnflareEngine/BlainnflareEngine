//
// Created by gorev on 08.10.2025.
//

#pragma once

#include <aliases.h>
#include <yaml-cpp/yaml.h>

namespace Blainn
{
static const std::string metaFormat = ".blainn";

struct ImportMeshData
{
    inline static ImportMeshData GetMeshData(const Path &absolutePath)
    {
        ImportMeshData data;
        YAML::Node node = YAML::LoadFile(absolutePath.string() + metaFormat);

        data.path = node["ModelPath"].as<std::string>();
        data.id = uuid::fromStrFactory(node["ID"].as<std::string>());
        data.convertToLH = node["ConvertToLH"].as<bool>();
        data.createMaterials = node["CreateMaterials"].as<bool>();
        return data;
    }

    uuid id;
    Path path;
    bool convertToLH;
    bool createMaterials;
};

struct ImportTextureData
{
};

struct ImportAudioData
{
};
} // namespace Blainn
