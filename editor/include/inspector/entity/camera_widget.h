//
// Created by WhoLeb on 25-Dec-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//


#pragma once
#include "component_widget_base.h"


namespace editor
{
class bool_input_field;
class float_input_field;

class camera_widget : public component_widget_base
{
public:
    explicit camera_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);
    void DeleteComponent() override;

protected slots:
    void OnPriorityChanged();
    void OnFOVChanged();
    void OnNearZChanged();
    void OnFarZChanged();

private:
    float_input_field *m_cameraPriority = nullptr;
    float_input_field *m_FOV = nullptr;
    float_input_field *m_nearZ = nullptr;
    float_input_field *m_farZ = nullptr;

    void paintEvent(QPaintEvent *event) override;
};
} // namespace editor