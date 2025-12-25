//
// Created by WhoLeb on 25-Dec-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#include "inspector/entity/camera_widget.h"

#include <QLayout>

#include "scene/EntityTemplates.h"
#include "components/CameraComponent.h"
#include "input-widgets/bool_input_field.h"

namespace editor {
    camera_widget::camera_widget(const Blainn::Entity &entity, QWidget *parent)
        : component_widget_base(entity, "Camera widget", parent)
    {
        m_isActiveCamera = new bool_input_field("Is Main Cam", false, this);

        layout()->addWidget(m_isActiveCamera);

        connect(m_isActiveCamera, &bool_input_field::toggled, this, &camera_widget::OnIsActiveChanged);
    }

    void camera_widget::OnIsActiveChanged(bool value)
    {
        auto* cam = m_entity.TryGetComponent<Blainn::CameraComponent>();
        if (cam) {
            cam->IsActiveCamera = value;
        }
    }

    void camera_widget::DeleteComponent()
    {
        if (m_entity.IsValid()) m_entity.RemoveComponent<Blainn::CameraComponent>();

        deleteLater();
    }
} // editor