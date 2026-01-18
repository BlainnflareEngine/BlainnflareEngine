#include "pch.h"

#include "ai/BTBuilder.h"
#include "ai/BehaviourTree.h"

using namespace Blainn;

// static bool ParseDecorators(sol::table node, BTBuilder& b)
// {
//     sol::object d = node["decorators"];

//     if (!d.valid() || d == sol::nil)
//         return false;

//     sol::table decorators = d.as<sol::table>();

//     for (eastl::size_t i = 1;; ++i)
//     {
//         sol::object o = decorators[i];
//         if (!o.valid() || o == sol::nil) 
//             break;

//         sol::table dec = o;
//         BTType type;
//         if (!ReadLuaBTType(dec, type))
//         {
//             BF_ERROR("ParseDecorators(): ReadLuaBTType didn't return type")
//             b.Reset();
//             return false;
//         }

//         sol::function fn = dec["fn"];

//         if (type == BTType::Negate)
//             b.AddNegate();
//         else if (type == BTType::Condition)
//         {
//             if (!fn.valid())
//             {
//                 BF_ERROR("ParseDecorators(): Condition node doesn't have condition function");
//                 b.Reset();
//                 return false;
//             }
//             b.AddCondition(fn);
//         }
//         else
//             BF_ERROR("ParseDecorators(): Unknown decorator type enum while parsing");
//             b.Reset();
//             return false;
//     }

//     return true;
// }

// static bool CalculateBT(sol::table node, BTBuilder& b)
// {
//     if (b.HasError())
//     {
//         BF_ERROR("CalculateBT(): BTBuilder has errors")
//         return false;
//     }

//     if (!node.valid())
//     {
//         BF_ERROR("CalculateBT(): invalid node table");
//         return false;
//     }

//     if (!ParseDecorators(node, b))
//     {
//         BF_ERROR("CalculateBT(): ParseDecorators has errors");
//         return false;
//     }

//     BTType type;
//     if (!ReadLuaBTType(node, type))
//     {
//         BF_ERROR("CalculateBT(): ReadLuaBTType didn't return type")
//         b.Reset();
//         return false;
//     }

//     switch (type) {
//         case BTType::Sequence:
//         case BTType::Selector:
//         {
//             if (type == BTType::Selector)
//                 b.AddSelector();
//             else if (type == BTType::Sequence)
//                 b.AddSequence();
            
//             sol::table children;
//             if (!ReadLuaChildrenTable(node, children))
//             {
//                 BF_ERROR("CalculateBT(): ReadLuaChildrenTable didn't return children")
//                 b.Reset();
//                 return false;
//             }

//             if (children.valid())
//             {
//                 for (eastl::size_t i = 1;; ++i)
//                 {
//                     sol::object childObj = children[i];
//                     if (!childObj.valid() || childObj.get_type() == sol::type::nil)
//                         break;

//                     if (!childObj.is<sol::table>())
//                     {
//                         BF_ERROR("BT parse: child must be a table");
//                         b.Reset();
//                         return false;
//                     }

//                     CalculateBT(childObj.as<sol::table>(), b);
//                     if (b.HasError())
//                     {
//                         BF_ERROR("CalculateBT(): CalculateBT has errors");
//                         return false;
//                     }
//                 }
//             }

//             b.End();
//             return false;
//         }
//         case BTType::Action:
//         {
//             sol::function fn;
//             if (!ReadLuaActionFn(node, fn))
//             {
//                 BF_ERROR("CalculateBT(): ReadLuaActionFn didn't return function");
//                 b.Reset();
//                 return false;
//             }
//             b.AddAction(fn);
//             return false;
//         }
//         case BTType::Negate:
//         {
//             b.AddNegate();

//             sol::table children;
//             if (!ReadLuaChildrenTable(node, children))
//             {
//                 BF_ERROR("CalculateBT(): ReadLuaChildrenTable didn't return table")
//                 b.Reset();
//                 return false;
//             }
//             if (!children.valid() || children[1] == sol::nil)
//             {
//                 BF_ERROR("CalculateBT(): Negate must have exactly one child")
//                 b.Reset();
//                 return false;
//             }
//             if (children[2] != sol::nil)
//             {
//                 BF_ERROR("CalculateBT(): Negate must have only one child")
//                 b.Reset();
//                 return false;               
//             }

//             sol::object c = children[1];
//             CalculateBT(c.as<sol::table>(), b);
//             return false;
//         }
//         default:
//             BF_ERROR("CalculateBT(): unknown node type enum while parsing");
//             b.Reset();
//             return false;
//     }
// }

// static bool BuildBTFromLua(sol::table rootTable, BTBuilder& builder, BehaviourTree& tree)
// {
//     sol::object btName = rootTable["name"];
    
//     if (!btName.valid())
//     {
//         BF_ERROR("BuildBTFromLua(): didn't find name for Behaviour Tree");
//         return false;
//     }
//     if (!btName.is<eastl::string>())
//     {
//         BF_ERROR("BuildBTFromLua(): name for BT is not string");
//         return false;
//     }
    
//     builder.SetBTName(btName.as<eastl::string>());

//     if (!CalculateBT(rootTable, builder))
//     {
//         BF_ERROR("BuildBTFromLua(): Failed to build BT");
//         return false;
//     }

//     tree = builder.Build();

//     return true;
// }

BTMap LoadBTs(sol::state& lua) // как примерно я предполагаю заргрузку из Lua, перевел в AISubsystem
{
    BTMap trees;

    sol::table btTable = lua["BehaviourTrees"];
    if (!btTable.valid())
        return trees;
    
    for (auto& kv : btTable)
    {
        sol::table bt = kv.second.as<sol::table>();
        BTBuilder builder;
        auto tree = builder.BuildFromLua(bt);

        if (!tree)
            continue;
        
        trees.emplace(tree->GetName(), eastl::move(tree));
    }

    return trees; // Должно возвращаться в AIController
}