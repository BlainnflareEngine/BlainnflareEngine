//
// Created by gorev on 13.11.2025.
//

#pragma once
#include "aliases.h"
#include "pch.h"

namespace Blainn
{
inline uuid GetID(const YAML::Node &node)
{
    return uuid::fromStrFactory(node["EntityID"].as<std::string>().c_str());
}

inline eastl::string GetTag(const YAML::Node &node)
{
    eastl::string tag = eastl::string();
    if (node["TagComponent"] && node["TagComponent"]["Tag"])
        tag = node["TagComponent"]["Tag"].as<std::string>().c_str();

    return tag;
}

inline bool HasTransform(const YAML::Node &node)
{
    if (!node || node.IsNull()) return false;

    if (node["TransformComponent"]) return true;

    return false;
}

inline TransformComponent GetTransform(const YAML::Node &node)
{
    TransformComponent transform;

    if (!node || node.IsNull())
    {
        BF_ERROR("Failed to parse transform component. Not found in .scene!");
        return transform;
    }

    if (node["Translation"])
    {
        auto translationNode = node["Translation"];
        if (translationNode["x"] && translationNode["y"] && translationNode["z"])
        {
            transform.Translation.x = translationNode["x"].as<float>();
            transform.Translation.y = translationNode["y"].as<float>();
            transform.Translation.z = translationNode["z"].as<float>();
        }
    }
    if (node["Rotation"])
    {
        auto rotationNode = node["Rotation"];
        if (rotationNode["x"] && rotationNode["y"] && rotationNode["z"])
        {
            float x = rotationNode["x"].as<float>();
            float y = rotationNode["y"].as<float>();
            float z = rotationNode["z"].as<float>();
            transform.SetRotationEuler(Vec3(x, y, z));
        }
    }

    if (node["Scale"])
    {
        auto scaleNode = node["Scale"];
        if (scaleNode["x"] && scaleNode["y"] && scaleNode["z"])
        {
            transform.Scale.x = scaleNode["x"].as<float>();
            transform.Scale.y = scaleNode["y"].as<float>();
            transform.Scale.z = scaleNode["z"].as<float>();
        }
    }

    return transform;
}
} // namespace Blainn
