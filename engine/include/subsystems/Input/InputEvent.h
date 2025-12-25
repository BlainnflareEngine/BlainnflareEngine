//
// Created by WhoLeb on 03-Nov-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#pragma once

#include "KeyCodes.h"
#include "EASTL/shared_ptr.h"

namespace Blainn
{
enum class InputEventType
{
    MouseMoved,
    MouseDelta,
    MouseButtonPressed,
    MouseButtonHeld,
    MouseButtonReleased,
    MouseScrolled,
    KeyPressed,
    KeyReleased,
    KeyHeld,
    COUNT
};

enum InputEventCategory
{
    None = 0,
    EventCategoryKeyboard       = 1 << 0,
    EventCategoryMouse          = 1 << 1,
    EventCategoryMouseButton    = 1 << 2
};

#define EVENT_CLASS_TYPE(type) static InputEventType GetStaticType() { return InputEventType::type; } \
                                virtual InputEventType GetEventType() const override { return GetStaticType(); }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

class InputEvent
{
public:
    virtual ~InputEvent() {}
    virtual InputEventType GetEventType() const = 0;
    virtual int GetCategoryFlags() const = 0;

    inline bool IsInCategory(InputEventCategory category)
    {
        return GetCategoryFlags() & category;
    }
};

using InputEventPointer = eastl::shared_ptr<InputEvent>;

struct InputEventPolicy
{
    static InputEventType getEvent(const InputEventPointer& inputEvent)
    {
        return inputEvent->GetEventType();
    }
};
}
