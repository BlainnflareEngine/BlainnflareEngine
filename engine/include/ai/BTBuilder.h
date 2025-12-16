#pragma once

#include "ai/Blackboard.h"
#include "ai/BT.h"

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
    struct PendingDecorator
    {
        BTType type;
        sol::function condition;
    };

public:
    BTBuilder& AddSequence();
    BTBuilder& AddSelector();
    BTBuilder& AddAction(sol::function fn);
    BTBuilder& AddNegate();
    BTBuilder& AddCondition(sol::function cond);
    BTBuilder& End();
    BTNodePtr Build();
    void Reset();
    bool HasError() const { return m_hasError; }
    bool SetBTName(const std::string& newName);
    const std::string& GetBTName() const { return m_btName; }

private:
    void AttachNode(BTNodePtr node);
    bool WrapDecorators(BTNodePtr& node);

    template <class TComposite>
    BTBuilder& OpenComposite()
    {
        auto node = std::make_unique<TComposite>();
        CompositeNode* raw = node.get();

        AttachNode(std::move(node));
        m_stack.push_back(raw);
        return *this;
    }

private:
    bool m_hasError = false;

    BTNodePtr m_root{};
    std::string m_btName = "None";
    std::vector<CompositeNode*> m_stack{}; // non owning pointers into nodes we own via unique_ptr
    std::vector<PendingDecorator> m_pendingDecorators;
};
} // namespace Blainn