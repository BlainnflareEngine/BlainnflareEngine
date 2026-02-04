#include <pch.h>
#include "Render/Camera.h"

#include "Engine.h"
#include "Render/FreyaMath.h"
#include "Subsystems/Input/InputSubsystem.h"
#include "Subsystems/Input/InputEvent.h"
#include "Subsystems/Input/KeyboardEvents.h"
#include "Subsystems/Input/MouseEvents.h"

namespace Blainn
{
Blainn::Camera::Camera()
    : m_nearZ(0.1f)
    , m_farZ(1000.0f)
    , m_fovYRad(0.0f)
    , m_aspectRatio(0.0f)
    , m_nearWindowHeight(0.0f)
    , m_farWindowHeight(0.0f)
{
    m_view = XMMatrixIdentity();
    m_persProj = XMMatrixIdentity();
    m_orthProj = XMMatrixIdentity();
    m_isDirty = true;
}

void Blainn::Camera::Update(float deltaTime)
{
    m_deltaTime = Engine::GetDeltaTime();
    if (!m_isDirty) return;

    // View matrix
    XMVECTOR pos = GetPosition();
    XMVECTOR target = pos + XMLoadFloat3(&m_forward);
    XMVECTOR up = XMLoadFloat3(&m_up);

    m_view = XMMatrixLookAtLH(pos, target, up);

    m_isDirty = false;
    XMFLOAT3 position = GetPosition3f();
}

void Blainn::Camera::Reset(float fovAngleYDegrees, float aspectRatio, float nearZ, float farZ)
{
    m_nearZ = nearZ;
    m_farZ = farZ;
    m_fovYRad = (fovAngleYDegrees / 180.0f) * XM_PI;
    m_aspectRatio = aspectRatio;

    m_nearWindowHeight = 2.0f * tanf(0.5f * m_fovYRad) * m_nearZ;
    m_farWindowHeight = 2.0f * tanf(0.5f * m_fovYRad) * m_farZ;

    UpdateProjectionMatrices();
    UpdateCameraFrustumCascadesSplits();

	// For frustum culling
	//BoundingFrustum::CreateFromMatrix(m_frustum, m_persProj);
}

void Camera::SetAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
    UpdateProjectionMatrices();
}


XMMATRIX Blainn::Camera::GetViewMatrix() const
{
    return m_view;
}

XMMATRIX Blainn::Camera::GetPerspectiveProjectionMatrix() const
{
    return m_persProj;
}

XMMATRIX Blainn::Camera::GetOrthoProjectionMatrix() const
{
    return m_orthProj;
}

void Camera::SetNearZ(float value)
{
    m_nearZ = value;
    m_nearWindowHeight = 2.0f * tanf(0.5f * m_fovYRad) * m_nearZ;

    UpdateProjectionMatrices();
    UpdateCameraFrustumCascadesSplits();
}

void Camera::SetFarZ(float value)
{
    m_farZ = value;
    m_farWindowHeight = 2.0f * tanf(0.5f * m_fovYRad) * m_farZ;
    
    UpdateProjectionMatrices();
    UpdateCameraFrustumCascadesSplits();
}

float Blainn::Camera::GetFovYRad() const
{
    return m_fovYRad;
}

float Blainn::Camera::GetFovXRad() const
{
    return 2.0f * atanf(GetFarWindowWidth() * 0.5f / m_farZ);
}

float Camera::GetFovDegrees() const
{
    return m_fovYRad * 180 / XM_PI;
}

void Camera::SetFovDegrees(float value)
{
    if (value <= 0.001f)
    {
        BF_ERROR("The field of view (FOV) cannot be zero. The FOV value is set to 1.");
        value = 1.0f;
    }

    m_fovYRad = (value / 180.0f) * XM_PI;

    UpdateProjectionMatrices();
}

XMFLOAT3 Blainn::Camera::GetPosition3f() const
{
    return m_position;
}

XMVECTOR Blainn::Camera::GetPosition() const
{
    return XMLoadFloat3(&m_position);
}

void Blainn::Camera::SetPosition(float x, float y, float z)
{
    m_position = XMFLOAT3(x, y, z);
    m_isDirty = true;
}

void Blainn::Camera::SetPosition(const XMFLOAT3 &v)
{
    m_position = v;
    m_isDirty = true;
}

void Camera::UpdateProjectionMatrices()
{
    m_persProj = XMMatrixPerspectiveFovLH(m_fovYRad, m_aspectRatio, m_nearZ, m_farZ);
    m_orthProj = XMMatrixOrthographicLH(GetNearWindowWidth(), GetNearWindowHeight(), m_nearZ, m_farZ);
}

void Camera::UpdateCameraFrustumCascadesSplits()
{
    // logarithmic partition
    const float minZ = m_nearZ;
    const float maxZ = m_farZ;

    const float range = maxZ - minZ;
    const float ratio = maxZ / minZ;

    for (int i = 0; i < MaxCascades; i++)
    {
        float p = (i + 1) / (float)(MaxCascades);
        float log = (float)(minZ * pow(ratio, p));
        float uniform = minZ + range * p;
        float d = 0.95f * (log - uniform) + uniform; // 0.95f - idk, just magic value
        m_frustumCascadesLevels[i] = ((d - minZ) / range) * maxZ;
    }
}

} // namespace Blainn