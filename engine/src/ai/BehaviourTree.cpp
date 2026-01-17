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

    //if (m_abortRequested)
    //{
    //    return BTStatus::Aborted;
    //}

    BTStatus s = m_root->Update(bb);

    if (s == BTStatus::Error)
        m_hasError = true;
    
    return s;
}

void Blainn::BehaviourTree::HardReset()
{
    m_abortRequested = false;
    m_hasError = false;

    if (m_root)
        m_root->Reset();
}

void Blainn::BehaviourTree::ClearState()
{
    m_abortRequested = false;
    m_hasError = false;

    if (m_root) m_root->ClearState();
}

void Blainn::BehaviourTree::RequestAbort()
{
    m_abortRequested = true;
}

bool Blainn::BehaviourTree::IsAborting() const
{
    return m_abortRequested;
}