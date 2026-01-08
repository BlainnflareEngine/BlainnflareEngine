#pragma once

#include <string>
#include <unordered_map>
#include <variant>

namespace Blainn
{

using BlackboardValue = std::variant<
    int,
    float,
    bool,
    std::string
>;

struct AIControllerComponent
{
    // std::string utilityProfile;

    // std::vector<std::string> behaviourTrees;

    // std::unordered_map<std::string, BlackboardValue> initialBlackboard;

    std::string ScriptPath;
};

} // namespace Blainn