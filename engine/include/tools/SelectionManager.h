//
// Created by WhoLeb on 13-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "aliases.h"
#include "EASTL/vector.h"
#include "eventpp/eventdispatcher.h"
#include "Input/InputSubsystem.h"

namespace Blainn
{
class SelectionManager
{
    using CallbackFn = eastl::function<void(uuid)>;
public:
    SelectionManager();
    ~SelectionManager();

    void SelectAt(const uint32_t x, const uint32_t y, bool keepSelection = false);
    void SelectUUID(uuid id, bool keepSelection = false);

    void DeselectAll();
    uuid GetSelectedUUID() const { return m_selectedUUID; }

    eventpp::CallbackList<void(uuid)> CallbackList;

public:
    bool EnablePicking = true;
private:
    // TODO
    eastl::vector<uuid> m_selectedUUIDs{20};

    uuid m_selectedUUID;

    Input::EventHandle m_lmbInputHandle;
};
}