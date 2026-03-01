#pragma once

#include "Render/DXHelpers.h"
#include "Subsystems/Input/KeyCodes.h"

namespace Blainn
{
class Camera
{
public:
    Camera();
    Camera(const Camera &other) = default;
    Camera &operator=(const Camera &other) = default;
    virtual ~Camera() = default;

    virtual void Update(float deltaTime);
    virtual void Reset(float fovAngleY, float aspectRatio, float nearZ, float farZ);
    void SetAspectRatio(float aspectRatio);

    XMMATRIX GetViewMatrix() const;
    XMMATRIX GetPerspectiveProjectionMatrix() const;
    XMMATRIX GetOrthoProjectionMatrix() const;
    // FORCEINLINE const BoundingFrustum& GetCameraFrustum() const { return m_frustum; }

    FORCEINLINE float GetNearZ() const
    {
        return m_nearZ;
    }
    FORCEINLINE float GetFarZ() const
    {
        return m_farZ;
    }
    void SetNearZ(float value);
    void SetFarZ(float value);

    float GetFovYRad() const;
    float GetFovXRad() const;
    float GetFovDegrees() const;
    void SetFovDegrees(float value);

    FORCEINLINE float GetNearWindowHeight() const
    {
        return m_nearWindowHeight;
    }
    FORCEINLINE float GetNearWindowWidth() const
    {
        return m_nearWindowHeight * m_aspectRatio;
    }
    FORCEINLINE float GetFarWindowHeight() const
    {
        return m_farWindowHeight;
    }
    FORCEINLINE float GetFarWindowWidth() const
    {
        return m_farWindowHeight * m_aspectRatio;
    }

    XMFLOAT3 GetPosition3f() const;
    XMVECTOR GetPosition() const;
    void SetPosition(float x, float y, float z);
    void SetPosition(const XMFLOAT3 &v);

    FORCEINLINE float GetFrustumCascadesLevel(UINT level) const
    {
        return m_frustumCascadesLevels[level];
    }

protected:
    void UpdateProjectionMatrices();
    void UpdateCameraFrustumCascadesSplits();

protected:
    XMFLOAT3 m_position = {0.0f, 0.0f, 0.0f};
    XMFLOAT3 m_right = {1.0f, 0.0f, 0.0f};
    XMFLOAT3 m_up = {0.0f, 1.0f, 0.0f};
    XMFLOAT3 m_forward = {0.0f, 0.0f, 1.0f};

    float m_nearZ;
    float m_farZ;
    float m_fovYRad;
    float m_aspectRatio;
    float m_nearWindowHeight;
    float m_farWindowHeight;

    float m_cameraSpeed = 0.01f;
    float m_cameraAcceleration = 2.0f;

    float m_deltaTime = 0.0f;
    bool m_bIsCameraActionsBinded = false;
    bool m_isDirty = false;
    bool m_bUseAcceleration = false;

    // BoundingFrustum m_frustum;

    XMMATRIX m_view = XMMatrixIdentity();
    XMMATRIX m_persProj = XMMatrixIdentity();
    XMMATRIX m_orthProj = XMMatrixIdentity();

    float m_frustumCascadesLevels[MaxCascades] = {0.0f, 0.0f, 0.0f, 0.0f};
};
} // namespace Blainn