#include <pch.h>
#include "ai/BTBuilder.h"

using namespace Blainn;

BTBuilder &BTBuilder::AddSequence() { return OpenComposite<SequenceNode>(); }

BTBuilder &BTBuilder::AddSelector() { return OpenComposite<SelectorNode>(); }

BTBuilder &BTBuilder::AddAction(sol::function fn)
{
    if (!fn.valid())
    {
        BF_ERROR("AddAction: invalid Lua function");
        m_hasError = true;
        return *this;
    }

    BTNodePtr node = std::make_unique<ActionNode>(std::move(fn));
    AttachNode(std::move(node));
    return *this;
}

BTBuilder &Blainn::BTBuilder::AddNegate()
{
    m_pendingDecorators.push_back({ BTType::Negate, std::move(sol::function{}) });
    return *this;
}

BTBuilder &Blainn::BTBuilder::AddCondition(sol::function cond)
{
    if (!cond.valid())
    {
        BF_ERROR("Condition decorator: invalid Lua function")
        m_hasError = true;
        return *this;
    }

    m_pendingDecorators.push_back({ BTType::Condition, cond });
    return *this;
}

BTBuilder &BTBuilder::End()
{
    if (m_stack.empty())
    {
        BF_ERROR("BTBuilder::End(): no open composite to End()");
        m_hasError = true;
        return *this;
    }
    m_stack.pop_back();
    return *this;
}

BTNodePtr BTBuilder::Build()
{
    if (m_hasError)
    {
        BF_ERROR("BTBuilder::build(): builder has errors");
        Reset();
        return nullptr;
    }
    if (!m_stack.empty())
    {
        BF_ERROR("BTBuilder::Build(): unclosed composites remain (missing End())");
        m_hasError = true;
        return nullptr;
    }
    if (!m_root)
    {
        BF_ERROR("BTBuilder::Build(): no root node");
        m_hasError = true;
        return nullptr;
    }

    return std::move(m_root);
}

void BTBuilder::Reset()
{
    m_root->Reset();
    m_stack.clear();
    m_pendingDecorators.clear();
    m_btName = "None";
    m_hasError = false;
}

void BTBuilder::AttachNode(BTNodePtr node)
{
    if (!node)
    {
        BF_ERROR("AttachNode: null");
        m_hasError = true;
        return;
    }

    if (!WrapDecorators(node))
        return;

    if (m_stack.empty())
    {
        // This node becomes root. Root can only be set once.
        if (m_root)
        {
            BF_ERROR("AttachNode: root already set");
            m_hasError = true;
            return;
        }
        m_root = std::move(node);
        return;
    }

    // Attach to top composite
    CompositeNode* parent = m_stack.back();
    parent->AddChild(std::move(node));
}

bool Blainn::BTBuilder::WrapDecorators(BTNodePtr &node)
{
    for (auto it = m_pendingDecorators.rbegin(); it != m_pendingDecorators.rend(); ++it)
    {
        if (!node)
        {
            BF_ERROR("WrapDecorators: node is null");
            return false;
        }

        switch (it->type)
        {
            case BTType::Negate:
                node = std::make_unique<NegateNode>(
                    std::move(node), it->condition
                );
                break;

            case BTType::Condition:
                node = std::make_unique<ConditionNode>(
                    std::move(node), it->condition
                );
                break;

            default:
                BF_ERROR("Unknown decorator type");
                return false;
        }
    }

    m_pendingDecorators.clear();
    return true;
}

bool Blainn::BTBuilder::SetBTName(const std::string& newName)
{
    if(newName.empty())
    {
        BF_ERROR("BT name is empty");
        return false;
    }
    m_btName = newName;
    return true;
}