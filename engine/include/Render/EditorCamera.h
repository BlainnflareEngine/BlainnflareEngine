//
// Created by WhoLeb on 25-Dec-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//


#pragma once
#include "Camera.h"
#include "Input/InputSubsystem.h"

namespace Blainn
{
    class EditorCamera : public Camera
    {
    public:
        virtual ~EditorCamera();
        virtual void Reset(float fovAngleY, float aspectRatio, float nearZ, float farZ) override;

        void Move(const KeyCode key);
        void AdjustRotation(float x, float y);
        void SetCameraProperties(const KeyCode key);

    private:
        void SetAcceleration(bool useAcceleration) { m_bUseAcceleration = useAcceleration; }
        void MoveRight(float d);
        void MoveForward(float d);
        void MoveUp(float d);

        // Rotation
        void AdjustYaw(float adjustYawValue);
        void AdjustPitch(float adjustPitchValue);

    private:
        eastl::vector<eastl::pair<Input::EventHandle, InputEventType>> m_inputEvents;
    };
} // Blainn