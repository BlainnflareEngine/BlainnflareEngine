#pragma once

#include "Render/DXHelpers.h"

namespace Blainn
{
    class Camera
    {
        public:
        Camera();
        /*virtual */~Camera() = default;
        
        void Update(float deltaTime);
        void Reset(float fovAngleY, float aspectRatio, float nearZ, float farZ);
        
        XMMATRIX GetViewMatrix() const;
        XMMATRIX GetPerspectiveProjectionMatrix() const;
        XMMATRIX GetOrthoProjectionMatrix() const;
        FORCEINLINE const BoundingFrustum& GetCameraFrustum() const { return m_frustum; }
        
        FORCEINLINE float GetNearZ() const { return m_nearZ; }
        FORCEINLINE float GetFarZ() const { return m_farZ; }
        
        float GetFovYRad() const;
        float GetFovXRad() const;
        
        FORCEINLINE float GetNearWindowHeight() const { return m_nearWindowHeight; }
        FORCEINLINE float GetNearWindowWidth() const{ return m_nearWindowHeight * m_aspectRatio; }
        FORCEINLINE float GetFarWindowHeight() const{ return m_farWindowHeight; }
        FORCEINLINE float GetFarWindowWidth() const { return m_farWindowHeight * m_aspectRatio; }
        
        
        XMFLOAT3 GetPosition3f() const;
        XMVECTOR GetPosition() const;
        void SetPosition(float x, float y, float z);
        void SetPosition(const XMFLOAT3& v);
        
        // Translation
        void MoveRight(float d);
        void MoveForward(float d);
        void MoveUp(float d);
        
        // Rotation
        void AdjustYaw(float adjustYawValue);
        void AdjustPitch(float adjustPitchValue);
        
        private:
        XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
        XMFLOAT3 m_right	= { 1.0f, 0.0f, 0.0f };
        XMFLOAT3 m_up		= { 0.0f, 1.0f, 0.0f };
        XMFLOAT3 m_forward	= { 0.0f, 0.0f, 1.0f };
        
        float m_nearZ;
        float m_farZ;
        float m_fovYRad;
        float m_aspectRatio;
        float m_nearWindowHeight;
        float m_farWindowHeight;
        
        bool m_isDirty = false;
        
        BoundingFrustum m_frustum;
        
        XMMATRIX m_view = XMMatrixIdentity();
        XMMATRIX m_persProj = XMMatrixIdentity();
        XMMATRIX m_orthProj = XMMatrixIdentity();
    };
} 