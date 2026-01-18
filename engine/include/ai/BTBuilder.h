#pragma once

#include "ai/Blackboard.h"
#include "ai/BehaviourTree.h"

namespace Blainn
{
enum class BTType : int
{
    Sequence  = 1,
    Selector  = 2,
    Action    = 3,
    Negate    = 4,
    Condition = 5,
    Error     = 6,
};

class BTBuilder
{
public:
    BTBuilder& AddSequence();
    BTBuilder& AddSelector();
    BTBuilder& AddAction(sol::function fn, sol::function onRes);
    BTBuilder &AddNegate(BTNodePtr child);
    BTBuilder &AddCondition(sol::function cond, BTNodePtr child);
    BTBuilder& End();

    bool ReadLuaBTType(sol::table node, BTType& outType);
    bool ReadLuaChildrenTable(sol::table node, sol::table& out);
    bool ReadLuaActionFn(sol::table node, sol::function& outFn, sol::function& outOnReset);
    bool ReadLuaConditionFn(sol::table node, sol::function &outFn);

    bool CalculateBT(sol::table node, BTNodePtr &outNode);
    BTNodePtr Build();
    eastl::unique_ptr<BehaviourTree> BuildFromLua(sol::table rootTable);
    void Reset();
    bool HasError() const { return m_hasError; }

private:
    void AttachNode(BTNodePtr node);

    template <class TComposite>
    BTBuilder& OpenComposite()
    {
        auto node = eastl::make_unique<TComposite>();
        CompositeNode* raw = node.get();

        AttachNode(eastl::move(node));
        m_stack.push_back(raw);
        return *this;
    }

private:
    bool m_hasError = false;

    BTNodePtr m_root{};
    eastl::vector<CompositeNode*> m_stack{}; // non owning pointers into nodes we own via unique_ptr
};
} // namespace Blainn