#pragma once

#include "BTNodes.h"

namespace Blainn
{
class BehaviourTree
{
public:
    BehaviourTree(eastl::string name, BTNodePtr root)
        : m_name(eastl::move(name)), m_root(eastl::move(root)) {}

    const eastl::string& GetName() const { return m_name; }

    BTStatus Update(Blackboard& bb);
    void HardReset();
    void ClearState();

    void RequestAbort();
    bool IsAborting() const;

private:
    eastl::string m_name;
    BTNodePtr m_root;
    bool m_abortRequested = false;
    bool m_hasError = false;
};

using BTMap = eastl::unordered_map<eastl::string, eastl::unique_ptr<BehaviourTree>>;

}
