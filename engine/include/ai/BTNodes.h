#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include <sol/sol.hpp>
#include "Blackboard.h"

namespace Blainn
{
enum class BTStatus : uint8_t 
{ 
    Success = 0, 
    Failure = 1, 
    Running = 2, 
    Aborted = 3, 
    Error = 4, // TODO: Make Error and Aborted logic for BT
};

struct BTNode
{
    virtual ~BTNode() = default;
    virtual BTStatus Update(Blackboard &bb) = 0;
    virtual void Reset() = 0; // TODO: Make reset implementation in BTNode children
};

using BTNodePtr = std::unique_ptr<BTNode>;

struct CompositeNode : BTNode
{
    std::vector<BTNodePtr> children;

    void AddChild(BTNodePtr n);
};

struct SequenceNode final : CompositeNode 
{
    BTStatus Update(Blackboard& bb) override;
};

struct SelectorNode final : CompositeNode
{
    BTStatus Update(Blackboard& bb) override;
};

struct ActionNode final : BTNode
{
    // Store a Lua function reference. It must remain valid (Lua state must outlive the tree).
    sol::function fn;

    explicit ActionNode(sol::function f);

    BTStatus Update(Blackboard& bb) override;
};

struct DecoratorNode : BTNode
{
    BTNodePtr child;
    sol::function condition; // optional
    
    explicit DecoratorNode(BTNodePtr c);
    explicit DecoratorNode(BTNodePtr c, sol::function cond = sol::function{});

protected:
    bool CheckCondition(Blackboard& bb, bool& outResult);
};

struct NegateNode final : DecoratorNode
{
    using DecoratorNode::DecoratorNode;
    BTStatus Update(Blackboard& bb) override;
};

struct ConditionNode final : DecoratorNode
{
    using DecoratorNode::DecoratorNode;
    BTStatus Update(Blackboard &bb) override;
};
} // namespace Blainn