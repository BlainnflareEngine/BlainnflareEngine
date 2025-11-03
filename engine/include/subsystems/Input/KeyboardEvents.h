//
// Created by WhoLeb on 03-Nov-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#pragma once

#include "InputEvent.h"
#include "KeyCodes.h"

namespace Blainn
{
class KeyboardEvent : public InputEvent
{
public:
    KeyboardEvent(const KeyCode key)
        : m_key(key)
    {}

    [[nodiscard]] KeyCode GetKey() const { return m_key; }
    [[nodiscard]] KeyCode GetKeyCode() const { return m_key; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard);
protected:
    KeyCode m_key;
};

class KeyPressedEvent final : public KeyboardEvent
{
public:
    KeyPressedEvent(const KeyCode key)
        : KeyboardEvent(key)
    {}

    EVENT_CLASS_TYPE(KeyPressed)
};

class KeyReleasedEvent final : public KeyboardEvent
{
public:
    KeyReleasedEvent(const KeyCode key)
        : KeyboardEvent(key)
    {}

    EVENT_CLASS_TYPE(KeyReleased)
};
}