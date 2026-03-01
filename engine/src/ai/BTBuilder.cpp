#include <pch.h>
#include "ai/BTBuilder.h"

using namespace Blainn;

BTBuilder &BTBuilder::AddSequence()
{
    return OpenComposite<SequenceNode>();
}

BTBuilder &BTBuilder::AddSelector()
{
    return OpenComposite<SelectorNode>();
}

BTBuilder &BTBuilder::AddAction(sol::function fn, sol::function onReset)
{
    if (!fn.valid())
    {
        BF_ERROR("AddAction: invalid Lua function");
        m_hasError = true;
        return *this;
    }
    if (!onReset.valid())
    {
        BF_ERROR("AddAction: invalid Lua onReset function");
        m_hasError = true;
        return *this;
    }

    BTNodePtr node = eastl::make_unique<ActionNode>(eastl::move(fn), eastl::move(onReset));
    AttachNode(eastl::move(node));
    return *this;
}

BTBuilder &Blainn::BTBuilder::AddNegate(BTNodePtr child)
{
    if (!child)
    {
        BF_ERROR("AddNegate: child is null");
        m_hasError = true;
        return *this;
    }

    BTNodePtr node = eastl::make_unique<NegateNode>(eastl::move(child), eastl::move(sol::function{}));
    AttachNode(eastl::move(node));
    return *this;
}

BTBuilder &Blainn::BTBuilder::AddCondition(sol::function cond, BTNodePtr child)
{
    if (!cond.valid())
    {
        BF_ERROR("AddCondition: invalid Lua function");
        m_hasError = true;
        return *this;
    }
    if (!child)
    {
        BF_ERROR("AddCondition: child is null");
        m_hasError = true;
        return *this;
    }

    BTNodePtr node = eastl::make_unique<ConditionNode>(eastl::move(child), eastl::move(cond));
    AttachNode(eastl::move(node));
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

bool BTBuilder::ReadLuaBTType(sol::table node, BTType &outType)
{
    sol::object t = node["type"];
    if (!t.valid() || t.get_type() == sol::type::nil)
    {
        BF_ERROR("ReadLuaBTType(): node missing 'type'");
        return false;
    }

    if (!t.is<int>())
    {
        BF_ERROR("ReadLuaBTType(): 'type' must be an int enum");
        return false;
    }

    outType = t.as<BTType>();
    return true;
}

bool BTBuilder::ReadLuaChildrenTable(sol::table node, sol::table &out)
{
    sol::object c = node["children"];
    if (!c.valid() || c.get_type() == sol::type::nil)
    {
        out = sol::table(); // empty allowed for composites, you can make this an error
        return true;
    }
    if (!c.is<sol::table>())
    {
        BF_ERROR("ReadLuaChildrenTable(): 'children' must be a table");
        return false;
    }
    out = c.as<sol::table>();
    return true;
}

bool BTBuilder::ReadLuaActionFn(sol::table node, sol::function &outFn, sol::function &outOnReset)
{
    sol::object f = node["fn"];
    sol::object onReset = node["onReset"];

    if (!f.valid() || f.get_type() == sol::type::nil)
    {
        BF_ERROR("ReadLuaActionFn(): Action node missing 'fn'");
        return false;
    }
    if (!f.is<sol::function>())
    {
        BF_ERROR("ReadLuaActionFn(): Action node 'fn' must be a function");
        return false;
    }

    if (!onReset.valid() || onReset.get_type() == sol::type::nil)
    {
        BF_ERROR("ReadLuaActionFn(): Action node missing 'onReset'");
        return false;
    }
    if (!onReset.is<sol::function>())
    {
        BF_ERROR("ReadLuaActionFn(): Action node 'onReset' must be a function");
        return false;
    }

    outFn = f.as<sol::function>();
    outOnReset = onReset.as<sol::function>();
    return true;
}

bool BTBuilder::ReadLuaConditionFn(sol::table node, sol::function &outFn)
{
    sol::object f = node["fn"];

    if (!f.valid() || f.get_type() == sol::type::nil)
    {
        BF_ERROR("ReadLuaConditionFn(): Condition node missing 'fn'");
        return false;
    }
    if (!f.is<sol::function>())
    {
        BF_ERROR("ReadLuaConditionFn(): Condition node 'fn' must be a function");
        return false;
    }

    outFn = f.as<sol::function>();
    return true;
}

bool BTBuilder::CalculateBT(sol::table node, BTNodePtr &outNode)
{
    if (HasError())
    {
        BF_ERROR("BTBuilder::CalculateBT(): BTBuilder has errors")
        return false;
    }

    if (!node.valid())
    {
        BF_ERROR("BTBuilder::CalculateBT(): invalid node table");
        return false;
    }

    BTType type;
    if (!ReadLuaBTType(node, type))
    {
        BF_ERROR("BTBuilder::CalculateBT(): ReadLuaBTType didn't return type")
        Reset();
        return false;
    }

    switch (type)
    {
    case BTType::Sequence:
    case BTType::Selector:
    {
        eastl::unique_ptr<CompositeNode> composite;

        if (type == BTType::Sequence)
        {
            composite = eastl::make_unique<SequenceNode>();
        }
        else // BTType::Selector
        {
            composite = eastl::make_unique<SelectorNode>();
        }

        sol::table children;
        if (!ReadLuaChildrenTable(node, children))
        {
            BF_ERROR("BTBuilder::CalculateBT(): ReadLuaChildrenTable didn't return children")
            Reset();
            return false;
        }

        if (children.valid())
        {
            for (eastl_size_t i = 1;; ++i)
            {
                sol::object childObj = children[i];
                if (!childObj.valid() || childObj.get_type() == sol::type::nil) break;

                if (!childObj.is<sol::table>())
                {
                    BF_ERROR("BTBuilder::BT parse: child must be a table");
                    Reset();
                    return false;
                }

                BTNodePtr childNode;
                if (!CalculateBT(childObj.as<sol::table>(), childNode))
                {
                    BF_ERROR("BTBuilder::CalculateBT(): failed to build child");
                    return false;
                }

                if (childNode)
                {
                    composite->AddChild(eastl::move(childNode));
                }
            }
        }

        outNode = eastl::move(composite);
        return true;
    }
    case BTType::Action:
    {
        sol::function fn;
        sol::function onReset;
        if (!ReadLuaActionFn(node, fn, onReset))
        {
            BF_ERROR("BTBuilder::CalculateBT(): ReadLuaActionFn didn't return function");
            Reset();
            return false;
        }
        outNode = eastl::make_unique<ActionNode>(eastl::move(fn), eastl::move(onReset));
        return true;
    }
    case BTType::Negate:
    {
        sol::table children;
        if (!ReadLuaChildrenTable(node, children))
        {
            BF_ERROR("BTBuilder::CalculateBT(): Negate - ReadLuaChildrenTable failed");
            Reset();
            return false;
        }

        if (!children.valid() || children[1] == sol::nil)
        {
            BF_ERROR("BTBuilder::CalculateBT(): Negate must have exactly one child");
            Reset();
            return false;
        }

        if (children[2] != sol::nil)
        {
            BF_ERROR("BTBuilder::CalculateBT(): Negate must have only one child");
            Reset();
            return false;
        }

        sol::object childObj = children[1];
        BTNodePtr childNode;
        if (!CalculateBT(childObj.as<sol::table>(), childNode))
        {
            BF_ERROR("BTBuilder::CalculateBT(): Negate - failed to build child");
            return false;
        }

        outNode = eastl::make_unique<NegateNode>(eastl::move(childNode), sol::function{});
        return true;
    }

    case BTType::Condition:
    {
        sol::function fn;
        if (!ReadLuaConditionFn(node, fn))
        {
            BF_ERROR("BTBuilder::CalculateBT(): ReadLuaConditionFn failed");
            Reset();
            return false;
        }

        sol::table children;
        if (!ReadLuaChildrenTable(node, children))
        {
            BF_ERROR("BTBuilder::CalculateBT(): Condition - ReadLuaChildrenTable failed");
            Reset();
            return false;
        }

        if (!children.valid() || children[1] == sol::nil)
        {
            BF_ERROR("BTBuilder::CalculateBT(): Condition must have exactly one child");
            Reset();
            return false;
        }

        if (children[2] != sol::nil)
        {
            BF_ERROR("BTBuilder::CalculateBT(): Condition must have only one child");
            Reset();
            return false;
        }

        sol::object childObj = children[1];
        BTNodePtr childNode;
        if (!CalculateBT(childObj.as<sol::table>(), childNode))
        {
            BF_ERROR("BTBuilder::CalculateBT(): Condition - failed to build child");
            return false;
        }

        outNode = eastl::make_unique<ConditionNode>(eastl::move(childNode), eastl::move(fn));
        return true;
    }

    default:
        BF_ERROR("BTBuilder::CalculateBT(): unknown node type enum while parsing");
        Reset();
        return false;
    }
}

BTNodePtr BTBuilder::Build()
{
    if (m_hasError)
    {
        BF_ERROR("BTBuilder::build(): builder has errors");
        Reset();
        return nullptr;
    }

    if (!m_root)
    {
        BF_ERROR("BTBuilder::Build(): no root node");
        m_hasError = true;
        return nullptr;
    }

    return eastl::move(m_root);
}

eastl::unique_ptr<BehaviourTree> BTBuilder::BuildFromLua(sol::table rootTable)
{
    sol::object nameObj = rootTable["name"];
    if (!nameObj.valid() || !nameObj.is<std::string>())
    {
        BF_ERROR("BT must have string name");
        return nullptr;
    }

    eastl::string name = nameObj.as<std::string>().c_str();

    BTNodePtr root;
    if (!CalculateBT(rootTable, root))
    {
        BF_ERROR("Failed to build BT: " + name);
        return nullptr;
    }

    if (!root)
    {
        BF_ERROR("BuildFromLua: root is null for BT: " + name);
        return nullptr;
    }

    return eastl::make_unique<BehaviourTree>(name, eastl::move(root));
}

void BTBuilder::Reset()
{
    if (m_root)
        m_root->Reset();
    m_stack.clear();
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

    if (m_stack.empty())
    {
        // This node becomes root. Root can only be set once.
        if (m_root)
        {
            BF_ERROR("AttachNode: root already set");
            m_hasError = true;
            return;
        }
        m_root = eastl::move(node);
        return;
    }

    // Attach to top composite
    CompositeNode *parent = m_stack.back();
    parent->AddChild(eastl::move(node));
}