//
// Created by WhoLeb on 25-Dec-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#include "entity/camera_widget.h"

#include <QLayout>

#include "scene/EntityTemplates.h"
#include "components/CameraComponent.h"
#include "input-widgets/bool_input_field.h"
#include "input-widgets/float_input_field.h"
#include "entity/mesh_widget.h"

namespace editor
{
camera_widget::camera_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Camera widget", parent)
{
    auto *cam = m_entity.TryGetComponent<Blainn::CameraComponent>();
    if (!cam) destroy();

    m_isActiveCamera = new bool_input_field("Main Camera", cam->IsActiveCamera, this);

    m_FOV = new float_input_field("FOV", cam->camera.GetFovDegrees(), this);

    m_nearZ = new float_input_field("Near Z\t", cam->camera.GetNearZ(), this);
    m_nearZ->SetDecimals(2);
    m_nearZ->SetMinValue(0.01f);

    m_farZ = new float_input_field("Far Z\t", cam->camera.GetFarZ(), this);
    m_farZ->SetDecimals(2);
    m_farZ->SetMinValue(10);

    layout()->addWidget(m_isActiveCamera);
    layout()->addWidget(m_FOV);
    layout()->addWidget(m_nearZ);
    layout()->addWidget(m_farZ);

    connect(m_isActiveCamera, &bool_input_field::toggled, this, &camera_widget::OnIsActiveChanged);
    connect(m_FOV, &float_input_field::EditingFinished, this, &camera_widget::OnFOVChanged);
    connect(m_nearZ, &float_input_field::EditingFinished, this, &camera_widget::OnNearZChanged);
    connect(m_farZ, &float_input_field::EditingFinished, this, &camera_widget::OnFarZChanged);
}

void camera_widget::OnIsActiveChanged(bool value)
{
    auto *cam = m_entity.TryGetComponent<Blainn::CameraComponent>();
    if (cam)
    {
        cam->IsActiveCamera = value;
    }
}

void camera_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponent<Blainn::CameraComponent>();

    deleteLater();
}

void camera_widget::OnFOVChanged()
{
    auto *cam = m_entity.TryGetComponent<Blainn::CameraComponent>();
    if (cam)
    {
        cam->camera.SetFovDegrees(m_FOV->GetValue());
    }
}

void camera_widget::OnFarZChanged()
{
    auto *cam = m_entity.TryGetComponent<Blainn::CameraComponent>();
    if (cam)
    {
        cam->camera.SetNearZ(m_farZ->GetValue());
    }
}

void camera_widget::OnNearZChanged()
{
    auto *cam = m_entity.TryGetComponent<Blainn::CameraComponent>();
    if (cam)
    {
        cam->camera.SetNearZ(m_nearZ->GetValue());
    }
}
} // namespace editor
