//
// Created by WhoLeb on 13-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "tools/SelectionManager.h"

#include "RenderSubsystem.h"

using namespace Blainn;

void SelectionManager::SelectAt(const uint32_t x, const uint32_t y, bool keepSelection)
{
    uuid newID = RenderSubsystem::GetInstance().GetUUIDAt(x, y);
    static const uuid emptyId{};

    if (!keepSelection)
        m_selectedUUIDs.clear();

    if (newID == emptyId)
    {
        CallbackList(emptyId);
        return;
    }

    CallbackList(newID);

    //m_selectedUUIDs.push_back(newID);
}

void SelectionManager::SelectUUID(uuid id, bool keepSelection)
{
    static const uuid emptyId{};

    /*
    if (!keepSelection)
        m_selectedUUIDs.clear();
        */

    if (id == emptyId)
    {
        CallbackList(emptyId);
        return;
    }

    CallbackList(id);

    //m_selectedUUIDs.push_back(id);
}


/*
void SelectionManager::DeselectAll()
{
    m_selectedUUIDs.clear();
}
*/
