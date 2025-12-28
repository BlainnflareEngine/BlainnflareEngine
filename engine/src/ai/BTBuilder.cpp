#include <pch.h>
#include "ai/BTBuilder.h"

using namespace Blainn;

BTBuilder &BTBuilder::AddSequence() { return OpenComposite<SequenceNode>(); }

BTBuilder &BTBuilder::AddSelector() { return OpenComposite<SelectorNode>(); }

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

    BTNodePtr node = std::make_unique<ActionNode>(std::move(fn), std::move(onReset));
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

bool BTBuilder::ReadLuaBTType(sol::table node, BTType& outType)
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

    outType = static_cast<BTType>(t.as<int>());
    return true;
}

bool BTBuilder::ReadLuaChildrenTable(sol::table node, sol::table& out)
{
    sol::object c = node["children"];
    if (!c.valid() || c.get_type() == sol::type::nil)
    {
        out = sol::table(); // empty -> allowed for composites (you can make this an error)
        return true;
    }
    if (!c.is<sol::table>())
    {
        BF_ERROR("ReadLuaChildrenTable(): 'children' must be a table");
        return false;
    }
    out = c.as<sol::table>();
    return;
}

bool BTBuilder::ReadLuaActionFn(sol::table node, sol::function& outFn, sol::function& outOnReset)
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
    return false;
}

bool BTBuilder::ParseDecorators(sol::table node)
{
    sol::object d = node["decorators"];

    if (!d.valid() || d == sol::nil)
        return false;

    sol::table decorators = d.as<sol::table>();

    for (std::size_t i = 1;; ++i)
    {
        sol::object o = decorators[i];
        if (!o.valid() || o == sol::nil) 
            break;

        sol::table dec = o;
        BTType type;
        if (!ReadLuaBTType(dec, type))
        {
            BF_ERROR("ParseDecorators(): ReadLuaBTType didn't return type")
            Reset();
            return false;
        }

        sol::function fn = dec["fn"];

        if (type == BTType::Negate)
            AddNegate();
        else if (type == BTType::Condition)
        {
            if (!fn.valid())
            {
                BF_ERROR("ParseDecorators(): Condition node doesn't have condition function");
                Reset();
                return false;
            }
            AddCondition(fn);
        }
        else
            BF_ERROR("ParseDecorators(): Unknown decorator type enum while parsing");
            Reset();
            return false;
    }

    return true;
}

bool BTBuilder::CalculateBT(sol::table node)
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

    if (!ParseDecorators(node))
    {
        BF_ERROR("BTBuilder::CalculateBT(): ParseDecorators has errors");
        return false;
    }

    BTType type;
    if (!ReadLuaBTType(node, type))
    {
        BF_ERROR("BTBuilder::CalculateBT(): ReadLuaBTType didn't return type")
        Reset();
        return false;
    }

    switch (type) {
        case BTType::Sequence:
        case BTType::Selector:
        {
            if (type == BTType::Selector)
                AddSelector();
            else if (type == BTType::Sequence)
                AddSequence();
            
            sol::table children;
            if (!ReadLuaChildrenTable(node, children))
            {
                BF_ERROR("BTBuilder::CalculateBT(): ReadLuaChildrenTable didn't return children")
                Reset();
                return false;
            }

            if (children.valid())
            {
                for (std::size_t i = 1;; ++i)
                {
                    sol::object childObj = children[i];
                    if (!childObj.valid() || childObj.get_type() == sol::type::nil)
                        break;

                    if (!childObj.is<sol::table>())
                    {
                        BF_ERROR("BTBuilder::BT parse: child must be a table");
                        Reset();
                        return false;
                    }

                    CalculateBT(childObj.as<sol::table>());
                    if (HasError())
                    {
                        BF_ERROR("BTBuilder::CalculateBT(): CalculateBT has errors");
                        return false;
                    }
                }
            }

            End();
            return false;
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
            AddAction(fn, onReset);
            return false;
        }
        case BTType::Negate:
        {
            AddNegate();

            sol::table children;
            if (!ReadLuaChildrenTable(node, children))
            {
                BF_ERROR("BTBuilder::CalculateBT(): ReadLuaChildrenTable didn't return table")
                Reset();
                return false;
            }
            if (!children.valid() || children[1] == sol::nil)
            {
                BF_ERROR("BTBuilder::CalculateBT(): Negate must have exactly one child")
                Reset();
                return false;
            }
            if (children[2] != sol::nil)
            {
                BF_ERROR("BTBuilder::CalculateBT(): Negate must have only one child")
                Reset();
                return false;               
            }

            sol::object c = children[1];
            CalculateBT(c.as<sol::table>());
            return false;
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

std::unique_ptr<BehaviourTree> BTBuilder::BuildFromLua(sol::table rootTable)
{
    sol::object nameObj = rootTable["name"];
    if (!nameObj.valid() || !nameObj.is<std::string>())
    {
        BF_ERROR("BT must have string name");
        return nullptr;
    }

    std::string name = nameObj.as<std::string>();

    if (!CalculateBT(rootTable))
    {
        BF_ERROR("Failed to build BT: " + name);
        return nullptr;
    }

    BTNodePtr root = Build();

    return std::make_unique<BehaviourTree>(name, std::move(root));
}

void BTBuilder::Reset()
{
    m_root->Reset();
    m_stack.clear();
    m_pendingDecorators.clear();
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