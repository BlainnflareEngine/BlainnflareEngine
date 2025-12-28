#include <pch.h>
#include "ai/BehaviourTree.h"

Blainn::BTStatus Blainn::BehaviourTree::Update(Blackboard &bb)
{
    bb.btAbortRequested = m_abortRequested;

    if (!m_root)
    {
        BF_ERROR("Behaviour Tree Update: no root exist");
        return BTStatus::Error;
    }

    if (m_abortRequested)
    {

        return BTStatus::Aborted;
    }

    return m_root->Update(bb);
}

void Blainn::BehaviourTree::Reset()
{
    m_abortRequested = false;
    if (m_root)
        m_root->Reset();
}
void Blainn::BehaviourTree::RequestAbort()
{
    m_abortRequested = true;
}

bool Blainn::BehaviourTree::IsAborting() const
{
    return m_abortRequested;
}