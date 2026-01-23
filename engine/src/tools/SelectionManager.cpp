//
// Created by WhoLeb on 13-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "tools/SelectionManager.h"

#include "RenderSubsystem.h"
#include "Input/InputEvent.h"
#include "Input/InputSubsystem.h"
#include "Input/MouseEvents.h"

using namespace Blainn;

SelectionManager::SelectionManager()
{
    m_lmbInputHandle = Input::AddEventListener(InputEventType::MouseButtonPressed,
                                               [this](const InputEventPointer &pEvent)
                                               {
                                                   auto event = static_cast<MouseButtonPressedEvent *>(pEvent.get());

                                                   if (event->GetMouseButton() == MouseButton::Left)
                                                   {
                                                       uint32_t xPos = event->GetX();
                                                       uint32_t yPos = event->GetY();
                                                       SelectAt(xPos, yPos);
                                                   }
                                               });
}

SelectionManager::~SelectionManager()
{
    Input::RemoveEventListener(InputEventType::MouseButtonPressed, m_lmbInputHandle);
}

void SelectionManager::SelectAt(const uint32_t x, const uint32_t y, bool keepSelection)
{
    BLAINN_PROFILE_FUNC();
    uuid newID = RenderSubsystem::GetInstance().GetUUIDAt(x, y);

    if (newID == m_selectedUUID) return;

    CallbackList(newID);
    /*m_selectedUUID = newID;
    for (auto &cbf : m_Callbacks)
        cbf(newID);*/
}

void SelectionManager::SelectUUID(uuid id, bool keepSelection)
{
    if (id == m_selectedUUID) return;
    m_selectedUUID = id;

    CallbackList(id);

    /*for (auto &cbf : m_Callbacks)
        cbf(id);*/
}

void SelectionManager::DeselectAll()
{
    m_selectedUUID = uuid{0, 0};
}
