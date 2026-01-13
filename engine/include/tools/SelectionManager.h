//
// Created by WhoLeb on 13-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "aliases.h"
#include "EASTL/vector.h"
#include "eventpp/eventdispatcher.h"

namespace Blainn
{
class SelectionManager
{
public:
    SelectionManager() = default;

    void SelectAt(const uint32_t x, const uint32_t y, bool keepSelection = false);
    void SelectUUID(uuid id, bool keepSelection = false);

    void DeselectAll();

    eventpp::CallbackList<void(uuid)> CallbackList;
private:
    // TODO
    eastl::vector<uuid> m_selectedUUIDs{20};
};
}