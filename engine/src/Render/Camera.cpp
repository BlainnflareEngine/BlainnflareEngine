#include <pch.h>
#include "Render/Camera.h"
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
    // Tempopary, there will be function Engine::GetDeltaTime()
    m_deltaTime = deltaTime;

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

    m_persProj = XMMatrixPerspectiveFovLH(m_fovYRad, m_aspectRatio, m_nearZ, m_farZ);
    m_orthProj = XMMatrixOrthographicLH(GetNearWindowWidth(), GetNearWindowHeight(), nearZ, farZ);

	// For frustum culling
	//BoundingFrustum::CreateFromMatrix(m_frustum, m_persProj);
}

void Camera::SetAspectRatio(float aspectRatio) {
	m_aspectRatio = aspectRatio;
    m_persProj = XMMatrixPerspectiveFovLH(m_fovYRad, m_aspectRatio, m_nearZ, m_farZ);
    m_orthProj = XMMatrixOrthographicLH(GetNearWindowWidth(), GetNearWindowHeight(), m_nearZ, m_farZ);
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

float Blainn::Camera::GetFovYRad() const
{
    return m_fovYRad;
}

float Blainn::Camera::GetFovXRad() const
{
    return 2.0f * atanf(GetFarWindowWidth() * 0.5f / m_farZ);
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

} // namespace Blainn