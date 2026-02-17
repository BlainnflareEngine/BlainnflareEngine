//
// Created by WhoLeb on 25-Dec-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//


#pragma once
#include "Camera.h"
#include "Input/InputSubsystem.h"
#include "EASTL/vector.h"

namespace Blainn
{
class EditorCamera : public Camera
{
public:
    struct RotationDelta
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    virtual ~EditorCamera();
    virtual void Reset(const ProjectionParams &params) override;

    void Move(const KeyCode key);
    void AdjustRotation(const RotationDelta &rotationDelta);
    void IncreaseSpeed();
    void DecreaseSpeed();
    void SetCameraProperties(const KeyCode key, InputEventType eventType);

private:
    void SetAcceleration(bool useAcceleration)
    {
        m_bUseAcceleration = useAcceleration;
    }
    void MoveRight(float d);
    void MoveForward(float d);
    void MoveUp(float d);

    // Rotation
    void AdjustYaw(float adjustYawValue);
    void AdjustPitch(float adjustPitchValue);

private:
    eastl::vector<eastl::pair<Input::EventHandle, InputEventType>> m_inputEvents;

    float m_editorSpeed = 1.0f;
    float m_maxEditorSpeed = 100.0f;
    float m_speedStep = 1.0f;
};
} // namespace Blainn
