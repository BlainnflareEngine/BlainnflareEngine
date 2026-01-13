//
// Created by WhoLeb on 03-Nov-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#pragma once

#include "InputEvent.h"

namespace Blainn
{
/*
 * This event provides information about the current position of the mouse after it moved
 * Not the delta, the delta would need to be calculated in a separate place.
 */
class MouseMovedEvent final : public InputEvent
{
public:
    MouseMovedEvent(const float x, const float y)
        : m_mouseX(x), m_mouseY(y) {}

    [[nodiscard]] inline float GetX() const { return m_mouseX; }
    [[nodiscard]] inline float GetY() const { return m_mouseY; }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse)
private:
    float m_mouseX, m_mouseY;
};

class MouseScrolledEvent final : public InputEvent
{
public:
    // A normal mouse has only the y component, but glfw provides x and y so it may be useful
    MouseScrolledEvent(const float xOffset, const float yOffset)
        : m_mouseScrolledX(xOffset)
        , m_mouseScrolledY(yOffset)
    {}

    [[nodiscard]] inline float GetXOffset() const { return m_mouseScrolledX; }
    [[nodiscard]] inline float GetYOffset() const { return m_mouseScrolledY; }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse)
private:
    float m_mouseScrolledX, m_mouseScrolledY;
};

class MouseButtonEvent : public InputEvent
{
public:

    [[nodiscard]] inline float GetX() const { return m_mouseX; }
    [[nodiscard]] inline float GetY() const { return m_mouseY; }

    [[nodiscard]] inline MouseButton GetMouseButton() const { return m_mouseButton; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryMouseButton)

protected:
    MouseButtonEvent(const MouseButton button, const float x, const float y)
        : m_mouseButton(button)
        , m_mouseX(x)
        , m_mouseY(y)
    {}

private:
    MouseButton m_mouseButton;
    float m_mouseX, m_mouseY;
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
public:
    MouseButtonPressedEvent(const MouseButton button, const float x, const float y)
        : MouseButtonEvent(button, x, y) {}

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonHeldEvent : public MouseButtonEvent
{
public:
    MouseButtonHeldEvent(const MouseButton button, const float x, const float y)
        : MouseButtonEvent(button, x, y) {}

    EVENT_CLASS_TYPE(MouseButtonHeld)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
    MouseButtonReleasedEvent(const MouseButton button, const float x, const float y)
        : MouseButtonEvent(button, x, y) {}

    EVENT_CLASS_TYPE(MouseButtonReleased)
};

}