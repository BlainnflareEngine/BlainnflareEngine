#pragma once

#include <string>
#include <memory>
#include "BTNodes.h"

namespace Blainn
{
class BehaviourTree
{
public:
    BehaviourTree(std::string name, BTNodePtr root)
        : m_name(std::move(name)), m_root(std::move(root)) {}

    const std::string& GetName() const { return m_name; }

    BTStatus Update(Blackboard& bb);
    void Reset();

    void RequestAbort();
    bool IsAborting() const;

private:
    std::string m_name;
    BTNodePtr m_root;
    bool m_abortRequested = false;
};

using BTMap = std::unordered_map<std::string, std::unique_ptr<BehaviourTree>>;

}
