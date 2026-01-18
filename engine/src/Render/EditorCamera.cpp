//
// Created by WhoLeb on 25-Dec-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#include "Render/EditorCamera.h"

#include "Input/InputSubsystem.h"
#include "Input/KeyboardEvents.h"
#include "Input/MouseEvents.h"

namespace Blainn {
    EditorCamera::~EditorCamera()
    {
        for (auto& [handle, type] : m_inputEvents)
            Input::RemoveEventListener(type, handle);
        m_inputEvents.clear();
    }

    void EditorCamera::Reset(float fovAngleY, float aspectRatio, float nearZ, float farZ) {
        Camera::Reset(fovAngleY, aspectRatio, nearZ, farZ);

        if (!m_bIsCameraActionsBinded)
        {
            m_inputEvents.push_back({Input::AddEventListener(InputEventType::KeyHeld,
                                    [this](const InputEventPointer &event)
                                    {
                                        const KeyPressedEvent *keyEvent = static_cast<const KeyPressedEvent*>(event.get());
                                        auto key = keyEvent->GetKey();

                                        if (Input::IsMouseButtonHeld(MouseButton::Right))
                                        {
                                            Move(key);
                                            SetCameraProperties(key);
                                        }

                                    }), InputEventType::KeyHeld});

            m_inputEvents.push_back({Input::AddEventListener(InputEventType::MouseDelta,
                                    [this](const InputEventPointer& event)
                                    {
                                        const MouseMovedEvent *mouseEvent = static_cast<const MouseMovedEvent *>(event.get());
                                        if (Input::IsMouseButtonHeld(MouseButton::Right))
                                        {
                                            AdjustRotation(mouseEvent->GetX(), mouseEvent->GetY());
                                        }
                                    }), InputEventType::MouseDelta});

            m_bIsCameraActionsBinded = true;
        }
    }

    void EditorCamera::Move(const KeyCode key)
    {
        float currCamSpeed = 0.1f * m_deltaTime * m_cameraSpeed;
        if (m_bUseAcceleration) currCamSpeed *= m_cameraAcceleration;

        switch (key)
        {
            case Blainn::KeyCode::A: MoveRight(-currCamSpeed);
                break;
            case Blainn::KeyCode::D: MoveRight(currCamSpeed);
                break;
            case Blainn::KeyCode::S: MoveForward(-currCamSpeed);
                break;
            case Blainn::KeyCode::W: MoveForward(currCamSpeed);
                break;
            case Blainn::KeyCode::Q: MoveUp(-currCamSpeed);
                break;
            case Blainn::KeyCode::E: MoveUp(currCamSpeed);
                break;
        }

        m_isDirty = true;
    }

    void EditorCamera::AdjustRotation(float x, float y)
    {
        AdjustYaw(0.001f * x * m_deltaTime);
        AdjustPitch(0.001f * y * m_deltaTime);
    }

    void EditorCamera::SetCameraProperties(const KeyCode key)
    {
        switch (key)
        {
            case Blainn::KeyCode::LeftShift:
                SetAcceleration(true);
                break;
        }
    }

    void Blainn::EditorCamera::MoveRight(float d)
    {
        // m_position += d * m_right;
        XMVECTOR s = XMVectorReplicate(d);
        XMVECTOR r = XMLoadFloat3(&m_right);
        XMVECTOR p = GetPosition();
        XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, r, p));
        m_isDirty = true;
    }

    void Blainn::EditorCamera::MoveForward(float d)
    {
        // m_position += d * m_forward;
        XMVECTOR s = XMVectorReplicate(d);
        XMVECTOR f = XMLoadFloat3(&m_forward);
        XMVECTOR p = GetPosition();
        XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, f, p));
        m_isDirty = true;
    }

    void Blainn::EditorCamera::MoveUp(float d)
    {
        // m_position += d * m_up
        XMVECTOR s = XMVectorReplicate(d);
        XMVECTOR u = XMLoadFloat3(&m_up);
        XMVECTOR p = GetPosition();
        XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, u, p));
        m_isDirty = true;
    }

    // Rotation around world's Y axis
    void Blainn::EditorCamera::AdjustYaw(float angle)
    {
        XMMATRIX R = XMMatrixRotationY(angle);
        XMStoreFloat3(&m_right,	  XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
        XMStoreFloat3(&m_up,	  XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
        XMStoreFloat3(&m_forward, XMVector3TransformNormal(XMLoadFloat3(&m_forward), R));
        m_isDirty = true;
    }

    // Rotation around X axis
    void Blainn::EditorCamera::AdjustPitch(float angle)
    {
        XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);
        XMStoreFloat3(&m_up,	  XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
        XMStoreFloat3(&m_forward, XMVector3TransformNormal(XMLoadFloat3(&m_forward), R));
        m_isDirty = true;
    }


} // Blainn