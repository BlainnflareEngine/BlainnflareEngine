#include <pch.h>
#include "ai/BehaviourTree.h"

Blainn::BTStatus Blainn::BehaviourTree::Update(Blackboard &bb)
{
    if (!m_root)
    {
        BF_ERROR("Behaviour Tree Update: no root exist");
        return BTStatus::Error;
    }
    return m_root->Update(bb);
}

void Blainn::BehaviourTree::Reset()
{
    if (m_root)
        m_root->Reset();
}