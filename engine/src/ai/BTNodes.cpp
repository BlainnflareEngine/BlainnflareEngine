#include <pch.h>
#include "ai/BTNodes.h"

using namespace Blainn;

void CompositeNode::AddChild(BTNodePtr n)
{
    if (!n)
    {
        BF_ERROR("Attempted to add null child");
        return;
    }

    children.emplace_back(eastl::move(n));
}

void CompositeNode::Reset()
{
    m_currentIndex = 0;
    for (auto& c : children)
        c->Reset();
}

void CompositeNode::ClearState()
{
    m_currentIndex = 0;
    for (auto &c : children)
        c->ClearState();
}

BTStatus SequenceNode::Update(Blackboard &bb)
{
    if (bb.btAbortRequested)
        return BTStatus::Aborted;

    while (m_currentIndex < children.size())
    {
        BTStatus s = children[m_currentIndex]->Update(bb);

        if (BTStatus::Success == s)
        {
            m_currentIndex++;
            continue;
        }
        return s;

        //switch (s)
        //{
        //    case BTStatus::Success:
        //        m_currentIndex++;
        //        break;

        //    case BTStatus::Running:
        //        return s;
        //        
        //    case BTStatus::Failure:
        //    case BTStatus::Aborted:
        //    case BTStatus::Error:
        //        return s;

        //    default:
        //        break;
        //}
    }

    return BTStatus::Success;
}

void SequenceNode::Reset()
{
    m_currentIndex = 0;
    CompositeNode::Reset();
}

void SequenceNode::ClearState()
{
    m_currentIndex = 0;
    CompositeNode::ClearState();
}

BTStatus SelectorNode::Update(Blackboard &bb)
{
    if (bb.btAbortRequested)
        return BTStatus::Aborted;

    while (m_currentIndex < children.size())
    {
        BTStatus s = children[m_currentIndex]->Update(bb);

        switch (s)
        {
            case BTStatus::Success:
                return BTStatus::Success;

            case BTStatus::Running:
                return BTStatus::Running;

            case BTStatus::Failure:
                m_currentIndex++;
                break;

            case BTStatus::Aborted:
            case BTStatus::Error:
                return s;
            
            default:
                break;
        }
    }

    return BTStatus::Failure;
}

void SelectorNode::Reset()
{
    m_currentIndex = 0;
    CompositeNode::Reset();
}

void SelectorNode::ClearState()
{
    m_currentIndex = 0;
    CompositeNode::ClearState();
}

ActionNode::ActionNode(sol::function f, sol::function onRes) : fn(eastl::move(f)), onReset(eastl::move(onRes))
{
    if (!fn.valid())
    {
        BF_ERROR("ActionNode got invalid Lua function");
        return;
    }
    if (!onReset.valid())
    {
        BF_ERROR("ActionNode got invalid Lua onReset function");
        return;
    }
}

BTStatus ActionNode::Update(Blackboard &bb)
{
    // Every Lua action must have logic if bb.btAbortRequested is true!
    // Convention: Lua action returns:
    //  - integer: 0=Failure, 1=Success, 2=Running, 3=Aborted OR
    //  - string: "failure"/"success"/"running"/"aborted", OR
    //  - nothing => treated as Error
    sol::protected_function pf = fn;
    sol::protected_function_result r = pf(&bb); // pass BB pointer

    if (!r.valid()) {
        sol::error err = r;
        BF_ERROR(eastl::string("Lua action error: ") + err.what());
        return BTStatus::Error;
    }

    if (bb.btAbortRequested) // TODO: Make sure it's working
        return BTStatus::Aborted;

    if (r.return_count() == 0) {
        return BTStatus::Success;
    }

    sol::object o = r.get<sol::object>();

    if (o.is<int>())
    {
        int v = o.as<int>();
        switch (v) 
        {
            case 0: return BTStatus::Failure;
            case 1: return BTStatus::Success;
            case 2: return BTStatus::Running;
            case 3: return BTStatus::Aborted;
            default:
                BF_ERROR("Lua action returned invalid int status");
                return BTStatus::Error;
        }
    }

    if (o.is<std::string>()) 
    {
        const eastl::string s = o.as<std::string>().c_str();
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

void Blainn::ActionNode::Reset()
{
    if (onReset.valid())
        onReset();
}

void ActionNode::ClearState()
{
    // У ActionNode нет внутреннего состояния для сброса
}

Blainn::DecoratorNode::DecoratorNode(BTNodePtr c) : child(eastl::move(c))
{
    if (!child)
    {
        BF_ERROR("DecoratorNode: child is null");
    }
}

Blainn::DecoratorNode::DecoratorNode(BTNodePtr c, sol::function cond) : child(eastl::move(c)), condition(eastl::move(cond))
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
        outResult = true; // нет условия то всегда true
        return true; // Путаница конечно, но я хз как сделать лучше. Тут возвращается результат выполнения, а в outRes результат сравнения.
    }

    sol::protected_function pf = condition;
    sol::protected_function_result r = pf(&bb);

    if (!r.valid())
    {
        sol::error err = r;
        auto errCmb = eastl::string("Decorator condition error: ") + err.what();
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

void DecoratorNode::Reset()
{
    if (child) child->Reset();
}

void DecoratorNode::ClearState()
{
    if (child) child->ClearState();
}

BTStatus Blainn::NegateNode::Update(Blackboard &bb)
{
    if (!child)
    {
        BF_ERROR("NegateNode: child is null");
        return BTStatus::Error;
    }
    
    if (bb.btAbortRequested)
        return BTStatus::Aborted;

    BTStatus s = child->Update(bb);

    switch (s) {
        case BTStatus::Success: return BTStatus::Failure;
        case BTStatus::Failure: return BTStatus::Success;
        default:
            return s;
    }
}

void Blainn::NegateNode::Reset()
{
    if (child) child->Reset();
}

void Blainn::NegateNode::ClearState()
{
    if (child) child->ClearState();
}

BTStatus Blainn::ConditionNode::Update(Blackboard &bb)
{
    bool condResult = false;
    if (!CheckCondition(bb, condResult))
        return BTStatus::Error;
    
    if (!condResult)
        return BTStatus::Failure;

    BTStatus s = child->Update(bb);

    //if (s == BTStatus::Error)
    //    return BTStatus::Error;

    return s;
}

void Blainn::ConditionNode::Reset()
{
    if (child) child->Reset();
}

void Blainn::ConditionNode::ClearState()
{
    if (child) child->ClearState();
}
