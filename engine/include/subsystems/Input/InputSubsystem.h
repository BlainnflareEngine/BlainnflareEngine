//
// Created by WhoLeb on 02-Nov-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#pragma once
#include "EASTL/shared_ptr.h"
#include "EASTL/unordered_map.h"
#include "InputEvent.h"
#include "KeyCodes.h"
#include "aliases.h"
#include "eventpp/eventqueue.h"
#include "helpers.h"

namespace Blainn
{
class Input
{
public:
    struct MousePosition
    {
        float X, Y;

        MousePosition operator+(const MousePosition& other) const
        {
            return {X + other.X, Y + other.Y};
        }

        MousePosition operator-(const MousePosition& other) const
        {
            return {X - other.X, Y - other.Y};
        }
        
        MousePosition &operator+=(const MousePosition &other)
        {
            X += other.X;
            Y += other.Y;
            return *this;
        }

        MousePosition &operator-=(const MousePosition &other)
        {
            X -= other.X;
            Y -= other.Y;
            return *this;
        }
    };

    static bool IsKeyPressed(KeyCode key);
    static bool IsKeyHeld(KeyCode key);
    static bool IsKeyDown(KeyCode key);
    static bool IsKeyReleased(KeyCode key);

    static bool IsMouseButtonPressed(MouseButton button);
    static bool IsMouseButtonHeld(MouseButton button);
    static bool IsMouseButtonDown(MouseButton button);
    static bool IsMouseButtonReleased(MouseButton button);

    static float GetMousePositionX();
    static float GetMousePositionY();
    static MousePosition GetMousePosition();
    static void UpdateMousePosition(float x, float y);
    static void UpdateMousePosition(MousePosition newPos);
    static void ResetMousePosition(float x, float y);
    static void ResetMousePosition(const MousePosition newPos);

    static void TransitionPressedKeys();
    static void TransitionPressedButtons();

    static void UpdateKeyState(KeyCode key, KeyState state);
    static void UpdateButtonState(MouseButton button, ButtonState state);

    // I recommend getting the type by using auto since the type is quite deep in the hierarchy
    // also it sucks ass, might need to make it a template function to get the type...
    static void AddEventListener(InputEventType eventType, eastl::function<void(const InputEventPointer &)> listener);
    static void ProcessEvents();

private:
    inline static eventpp::EventQueue<InputEventType, void(const InputEventPointer &), InputEventPolicy>
        s_inputEventQueue;

    inline static eastl::unordered_map<KeyCode, KeyState> s_keyStates;
    inline static eastl::unordered_map<MouseButton, ButtonState> s_mouseButtonStates;

    inline static MousePosition s_mousePosition;
    inline static MousePosition s_mouseDelta;
};
} // namespace Blainn