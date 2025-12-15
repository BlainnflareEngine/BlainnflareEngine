#include <pch.h>
#include "ai/BT.h"
#include "BT.h"

using namespace Blainn;

void CompositeNode::AddChild(BTNodePtr n)
{
    if (!n)
    {
        BF_ERROR("Attempted to add null child");
        return;
    }

    children.emplace_back(std::move(n));
}

BTStatus SequenceNode::Tick(Blackboard &bb)
{
    for (auto& c : children)
    {
        BTStatus s = c->Tick(bb);
        if (s != BTStatus::Success) return s;
    }

    return BTStatus::Success;
}

BTStatus SelectorNode::Tick(Blackboard &bb)
{
    for (auto& c : children)
    {
        BTStatus s = c->Tick(bb);
        if (s == BTStatus::Success) return BTStatus::Success;
        if (s == BTStatus::Running) return BTStatus::Running;
    }

    return BTStatus::Failure;
}

ActionNode::ActionNode(sol::function f) : fn(std::move(f))
{
    if (!fn.valid())
    {
        BF_ERROR("ActionNode got invalid Lua function");
        return;
    }
}

BTStatus ActionNode::Tick(Blackboard &bb)
{
    // Convention: Lua action returns:
    //  - integer: 0=Failure, 1=Success, 2=Running, 3=Aborted OR
    //  - string: "failure"/"success"/"running"/"aborted", OR
    //  - nothing => treated as Error
    sol::protected_function pf = fn;
    sol::protected_function_result r = pf(&bb); // pass BB pointer (or userdata) as you prefer

    if (!r.valid()) {
        sol::error err = r;
        BF_ERROR(std::string("Lua action error: ") + err.what());
        return BTStatus::Error;
    }

    if (r.return_count() == 0) {
        return BTStatus::Success;
    }

    sol::object o = r.get<sol::object>();
    if (o.is<int>()) {
        int v = o.as<int>();
        switch (v) {
            case 0: return BTStatus::Failure;
            case 1: return BTStatus::Success;
            case 2: return BTStatus::Running;
            case 3: return BTStatus::Aborted;
            default:
                BF_ERROR("Lua action returned invalid int status");
                return BTStatus::Error;
        }
    }

    if (o.is<std::string>()) {
        const std::string s = o.as<std::string>();
        if (s == "failure") return BTStatus::Failure;
        if (s == "success") return BTStatus::Success;
        if (s == "running") return BTStatus::Running;
        if (s == "aborted") return BTStatus::Aborted;
        BF_ERROR("Lua action returned invalid string status");
        return BTStatus::Error;
    }

    BF_ERROR("Lua action returned unsupported status type");
    return BTStatus::Error;
}

Blainn::DecoratorNode::DecoratorNode(BTNodePtr c) : child(std::move(c))
{
    if (!child)
    {
        BF_ERROR("DecoratorNode: child is null")
    }
}

Blainn::DecoratorNode::DecoratorNode(BTNodePtr c, sol::function cond) : child(std::move(c)), condition(std::move(cond))
{
    if (!child)
    {
        BF_ERROR("DecoratorNode: child is null");
    }
}

bool Blainn::DecoratorNode::CheckCondition(Blackboard &bb, bool& outResult)
{
    outResult = false;

    if (!child)
    {
        BF_ERROR("DecoratorNode: child is null");
        return false;
    }

    if (!condition.valid())
    {
        outResult = true; // нет условия - всегда true
        return true; // Путаница конечно, но я хз как сделать лучше. Тут возвращается результат выполнения, а в outRes результат сравнения.
    }

    sol::protected_function pf = condition;
    sol::protected_function_result r = pf(&bb);

    if (!r.valid())
    {
        sol::error err = r;
        auto errCmb = std::string("Decorator condition error: ") + err.what();
        BF_ERROR(errCmb);
        return false;
    }

    if (!r.return_count())
    {
        BF_ERROR("Decorator condition must return bool");
        return false;
    }

    sol::object o = r.get<sol::object>();
    if (!o.is<bool>())
    {
        BF_ERROR("Decorator condition must return bool");
        return false;
    }

    outResult = o.as<bool>();
    return true;
}

BTStatus Blainn::NegateNode::Tick(Blackboard &bb)
{
    bool condResult = false;
    if (!CheckCondition(bb, condResult))
        return BTStatus::Error;

    if (!condResult)
        return BTStatus::Failure;

    BTStatus s = child->Tick(bb);

    switch (s) {
        case BTStatus::Success: return BTStatus::Failure;
        case BTStatus::Failure: return BTStatus::Success;
        case BTStatus::Running: return BTStatus::Running;
    }

    BF_ERROR("NegateNode: invalid BTStatus");
    return BTStatus::Error;
}

BTStatus Blainn::ConditionNode::Tick(Blackboard &bb)
{
    bool condResult = false;
    if (!CheckCondition(bb, condResult))
        return BTStatus::Error;
    
    if (!condResult)
        return BTStatus::Failure;

    return child->Tick(bb);
}
