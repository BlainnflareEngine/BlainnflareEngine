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
    Error = 4, // TODO: Make Error logic for BT
};

struct BTNode
{
    virtual ~BTNode() = default;
    virtual BTStatus Update(Blackboard &bb) = 0;
    virtual void Reset() = 0; // NOTE: Check if it's really working
};

using BTNodePtr = std::unique_ptr<BTNode>;

struct CompositeNode : BTNode
{
    std::vector<BTNodePtr> children;
    size_t m_currentIndex = 0;

    void AddChild(BTNodePtr n);
    void Reset() override;
};

struct SequenceNode final : CompositeNode 
{
    size_t m_currentIndex = 0;

    BTStatus Update(Blackboard& bb) override;
    void Reset() override;
};

struct SelectorNode final : CompositeNode
{
    size_t m_currentIndex = 0;

    BTStatus Update(Blackboard& bb) override;
    void Reset() override;
};

struct ActionNode final : BTNode
{
    // Store a Lua function reference. It must remain valid (Lua state must outlive the tree).
    sol::function fn;
    sol::function onReset;

    explicit ActionNode(sol::function f, sol::function onReset);

    BTStatus Update(Blackboard& bb) override;
    void Reset() override;
};

struct DecoratorNode : BTNode
{
    BTNodePtr child;
    sol::function condition; // optional
    
    explicit DecoratorNode(BTNodePtr c);
    explicit DecoratorNode(BTNodePtr c, sol::function cond = sol::function{});
    void Reset() override;

protected:
    bool CheckCondition(Blackboard& bb, bool& outResult);
};

struct NegateNode final : DecoratorNode
{
    using DecoratorNode::DecoratorNode;
    BTStatus Update(Blackboard& bb) override;
    void Reset() override;
};

struct ConditionNode final : DecoratorNode
{
    using DecoratorNode::DecoratorNode;
    BTStatus Update(Blackboard &bb) override;
    void Reset() override;
};
} // namespace Blainn