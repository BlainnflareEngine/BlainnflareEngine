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
} // namespace Blainn
