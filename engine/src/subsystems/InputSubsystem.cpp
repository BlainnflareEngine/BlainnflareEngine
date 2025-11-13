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
    return s_mouseButtonStates.find(button) != s_mouseButtonStates.end() && s_mouseButtonStates[button] == ButtonState::Pressed;
}

bool Blainn::Input::IsMouseButtonHeld(MouseButton button)
{
    return s_mouseButtonStates.find(button) != s_mouseButtonStates.end() && s_mouseButtonStates[button] == ButtonState::Held;
}

bool Blainn::Input::IsMouseButtonDown(MouseButton button)
{
    return IsMouseButtonPressed(button) || IsMouseButtonHeld(button);
}

bool Blainn::Input::IsMouseButtonReleased(MouseButton button)
{
    return s_mouseButtonStates.find(button) != s_mouseButtonStates.end() && s_mouseButtonStates[button] == ButtonState::Released;
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
    s_mousePosition = newPos;
    s_inputEventQueue.enqueue(InputEventType::MouseMoved, eastl::make_shared<MouseMovedEvent>(newPos.X, newPos.Y));
}

void Blainn::Input::TransitionPressedKeys()
{
    for (const auto& [key, keyState] : s_keyStates)
    {
        if (keyState == KeyState::Pressed)
            UpdateKeyState(key, KeyState::Held);
    }
}

void Blainn::Input::TransitionPressedButtons()
{
    for (const auto& [button, buttonState] : s_mouseButtonStates)
    {
        if (buttonState == ButtonState::Pressed)
            UpdateButtonState(button, ButtonState::Held);
    }
}

void Blainn::Input::UpdateKeyState(KeyCode key, KeyState state)
{
    s_keyStates[key] = state;
    switch (state)
    {
    case KeyState::Pressed:
        s_inputEventQueue.enqueue(eastl::make_shared<KeyPressedEvent>(key));
        return;
    case KeyState::Released:
        s_inputEventQueue.enqueue(eastl::make_shared<KeyReleasedEvent>(key));
        return;
    case KeyState::Held:
        // Held should probably not generate event, this can be handled in updates
        // of the scripts and using the Input::IsKeyHeld function.
        return;
    default:
        return;
    }
}

void Blainn::Input::UpdateButtonState(MouseButton button, ButtonState state)
{
    s_mouseButtonStates[button] = state;
    switch (state)
    {
    case ButtonState::Pressed:
        s_inputEventQueue.enqueue(eastl::make_shared<MouseButtonPressedEvent>(button));
        return;
    case ButtonState::Released:
        s_inputEventQueue.enqueue(eastl::make_shared<MouseButtonReleasedEvent>(button));
        return;
    case ButtonState::Held:
        // Held should probably not generate event, this can be handled in updates
        // of the scripts and using the Input::IsButtonHeld function.
        return;
    default:
        return;
    }
}

void Blainn::Input::AddEventListener(InputEventType eventType,
                                     eastl::function<void(const InputEventPointer &)> listener)
{
    s_inputEventQueue.appendListener(eventType, listener);
}

void Blainn::Input::ProcessEvents()
{
    s_inputEventQueue.process();
}