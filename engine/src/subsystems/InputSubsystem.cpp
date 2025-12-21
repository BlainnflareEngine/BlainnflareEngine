//
// Created by WhoLeb on 03-Nov-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#include "Input/InputSubsystem.h"

#include "Input/KeyboardEvents.h"
#include "Input/MouseEvents.h"

bool Blainn::Input::IsKeyPressed(KeyCode key)
{
    return s_keyStates.find(key) != s_keyStates.end() && s_keyStates[key] == KeyState::Pressed;
}

bool Blainn::Input::IsKeyHeld(KeyCode key)
{
    return s_keyStates.find(key) != s_keyStates.end() && s_keyStates[key] == KeyState::Held;
}

bool Blainn::Input::IsKeyDown(KeyCode key)
{
    return IsKeyPressed(key) || IsKeyHeld(key);
}

bool Blainn::Input::IsKeyReleased(KeyCode key)
{
    return s_keyStates.find(key) != s_keyStates.end() && s_keyStates[key] == KeyState::Released;
}

bool Blainn::Input::IsMouseButtonPressed(MouseButton button)
{
    return s_mouseButtonStates.find(button) != s_mouseButtonStates.end()
           && s_mouseButtonStates[button] == ButtonState::Pressed;
}

bool Blainn::Input::IsMouseButtonHeld(MouseButton button)
{
    return s_mouseButtonStates.find(button) != s_mouseButtonStates.end()
           && s_mouseButtonStates[button] == ButtonState::Held;
}

bool Blainn::Input::IsMouseButtonDown(MouseButton button)
{
    return IsMouseButtonPressed(button) || IsMouseButtonHeld(button);
}

bool Blainn::Input::IsMouseButtonReleased(MouseButton button)
{
    return s_mouseButtonStates.find(button) != s_mouseButtonStates.end()
           && s_mouseButtonStates[button] == ButtonState::Released;
}


float Blainn::Input::GetMousePositionX()
{
    return s_mousePosition.X;
}

float Blainn::Input::GetMousePositionY()
{
    return s_mousePosition.Y;
}

auto Blainn::Input::GetMousePosition() -> Blainn::Input::MousePosition
{
    return s_mousePosition;
}

void Blainn::Input::UpdateMousePosition(const float x, const float y)
{
    UpdateMousePosition({x, y});
}

void Blainn::Input::UpdateMousePosition(const MousePosition newPos)
{
    s_mouseDelta = newPos - s_mousePosition;

    s_mousePosition = newPos;
    s_inputEventQueue.enqueue(InputEventType::MouseMoved, eastl::make_shared<MouseMovedEvent>(newPos.X, newPos.Y));
    s_inputEventQueue.enqueue(InputEventType::MouseDelta,
                              eastl::make_shared<MouseMovedEvent>(s_mouseDelta.X, s_mouseDelta.Y));
}


void Blainn::Input::ResetMousePosition(float x, float y)
{
    ResetMousePosition({x, y});
}


void Blainn::Input::ResetMousePosition(const MousePosition newPos)
{
    s_mousePosition = newPos;
    s_mouseDelta = {0, 0};
}

void Blainn::Input::TransitionPressedKeys()
{
    for (const auto &[key, keyState] : s_keyStates)
    {
        if (keyState == KeyState::Pressed || keyState == KeyState::Held) UpdateKeyState(key, KeyState::Held);
    }
}

void Blainn::Input::TransitionPressedButtons()
{
    for (const auto &[button, buttonState] : s_mouseButtonStates)
    {
        if (buttonState == ButtonState::Pressed || buttonState == ButtonState::Held)
            UpdateButtonState(button, ButtonState::Held);
    }
}

void Blainn::Input::UpdateKeyState(KeyCode key, KeyState state)
{
    switch (state)
    {
    case KeyState::Pressed:
        if (s_keyStates[key] == KeyState::Held) return;
        s_keyStates[key] = state;
        BF_DEBUG("Key Pressed 111 {}", static_cast<int>(key))
        s_inputEventQueue.enqueue(eastl::make_shared<KeyPressedEvent>(key));
        return;
    case KeyState::Released:
        BF_DEBUG("Key Released  11111 {}", static_cast<int>(key))
        s_keyStates[key] = state;
        s_inputEventQueue.enqueue(eastl::make_shared<KeyReleasedEvent>(key));
        return;
    case KeyState::Held:
        s_keyStates[key] = state;
        BF_DEBUG("Key Held {}", static_cast<int>(key))
        s_inputEventQueue.enqueue(eastl::make_shared<KeyHeldEvent>(key));
        return;
    default:
        return;
    }
}

void Blainn::Input::UpdateButtonState(MouseButton button, ButtonState state)
{
    switch (state)
    {
    case ButtonState::Pressed:
        if (s_mouseButtonStates[button] == ButtonState::Held) return;
        s_mouseButtonStates[button] = state;
        s_inputEventQueue.enqueue(eastl::make_shared<MouseButtonPressedEvent>(button));
        return;
    case ButtonState::Released:
        s_mouseButtonStates[button] = state;
        s_inputEventQueue.enqueue(eastl::make_shared<MouseButtonReleasedEvent>(button));
        return;
    case ButtonState::Held:
        s_mouseButtonStates[button] = state;
        s_inputEventQueue.enqueue(eastl::make_shared<MouseButtonHeldEvent>(button));
        return;
    default:
        return;
    }
}

auto Blainn::Input::AddEventListener(InputEventType eventType,
                                     eastl::function<void(const InputEventPointer &)> listener) -> EventHandle
{
    return s_inputEventQueue.appendListener(eventType, listener);
}

bool Blainn::Input::RemoveEventListener(InputEventType eventType, EventHandle handle)
{
    return s_inputEventQueue.removeListener(eventType, handle);
}

void Blainn::Input::ProcessEvents()
{
    TransitionPressedKeys();
    TransitionPressedButtons();

    s_inputEventQueue.process();
}