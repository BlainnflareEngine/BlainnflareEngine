#include "pch.h"

#include "ai/BTBuilder.h"

using namespace Blainn;

static bool ReadLuaBTType(sol::table node, BTType& outType)
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

static bool ReadLuaChildrenTable(sol::table node, sol::table& out)
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

static bool ReadLuaActionFn(sol::table node, sol::function& outFn)
{
    sol::object f = node["fn"];
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

    outFn = f.as<sol::function>();
    return false;
}

static bool ParseDecorators(sol::table node, BTBuilder& b)
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
            b.Reset();
            return false;
        }

        sol::function fn = dec["fn"];

        if (type == BTType::Negate)
            b.AddNegate();
        else if (type == BTType::Condition)
        {
            if (!fn.valid())
            {
                BF_ERROR("ParseDecorators(): Condition node doesn't have condition function");
                b.Reset();
                return false;
            }
            b.AddCondition(fn);
        }
        else
            BF_ERROR("ParseDecorators(): Unknown decorator type enum while parsing");
            b.Reset();
            return false;
    }

    return true;
}

static void BuildBTFromLua(sol::table node, BTBuilder& b)
{
    if (b.HasError())
    {
        BF_ERROR("BuildBTFromLua(): BTBuilder has errors")
        return;
    }

    if (!node.valid())
    {
        BF_ERROR("BuildBTFromLua(): invalid node table");
        return;
    }

    if (!ParseDecorators(node, b))
    {
        BF_ERROR("BuildBTFromLua(): ParseDecorators has errors");
        return;
    }

    BTType type;
    if (!ReadLuaBTType(node, type))
    {
        BF_ERROR("BuildBTFromLua(): ReadLuaBTType didn't return type")
        b.Reset();
        return;
    }

    switch (type) {
        case BTType::Sequence:
        case BTType::Selector:
        {
            if (type == BTType::Selector)
                b.AddSelector();
            else if (type == BTType::Sequence)
                b.AddSequence();
            
            sol::table children;
            if (!ReadLuaChildrenTable(node, children))
            {
                BF_ERROR("BuildBTFromLua(): ReadLuaChildrenTable didn't return children")
                b.Reset();
                return;
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
                        BF_ERROR("BT parse: child must be a table");
                        b.Reset();
                        return;
                    }

                    BuildBTFromLua(childObj.as<sol::table>(), b);
                    if (b.HasError())
                    {
                        BF_ERROR("BuildBTFromLua(): BuildBTFromLua has errors");
                        return;
                    }
                }
            }

            b.End();
            return;
        }
        case BTType::Action:
        {
            sol::function fn;
            if (!ReadLuaActionFn(node, fn))
            {
                BF_ERROR("BuildBTFromLua(): ReadLuaActionFn didn't return function");
                b.Reset();
                return;
            }
            b.AddAction(fn);
            return;
        }
        case BTType::Negate:
        {
            b.AddNegate();

            sol::table children;
            if (!ReadLuaChildrenTable(node, children))
            {
                BF_ERROR("BuildBTFromLua(): ReadLuaChildrenTable didn't return table")
                b.Reset();
                return;
            }
            if (!children.valid() || children[1] == sol::nil)
            {
                BF_ERROR("BuildBTFromLua(): Negate must have exactly one child")
                b.Reset();
                return;
            }
            if (children[2] != sol::nil)
            {
                BF_ERROR("BuildBTFromLua(): Negate must have only one child")
                b.Reset();
                return;               
            }

            sol::object c = children[1];
            BuildBTFromLua(c.as<sol::table>(), b);
            return;
        }
        default:
            BF_ERROR("BuildBTFromLua(): unknown node type enum while parsing");
            b.Reset();
            return;
    }
}