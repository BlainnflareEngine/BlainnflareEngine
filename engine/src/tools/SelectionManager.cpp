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

    CallbackList(newID);
    m_selectedUUID = newID;
}

void SelectionManager::SelectUUID(uuid id, bool keepSelection)
{
    CallbackList(id);
    m_selectedUUID = id;
}
