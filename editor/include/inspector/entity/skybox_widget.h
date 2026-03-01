#pragma once

#include "component_widget_base.h"

namespace editor
{
class path_input_field;

class skybox_widget : public component_widget_base
{
public:
    explicit skybox_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

    void DeleteComponent() override;

private:
    path_input_field *m_texture_input;

    void UpdatePath();
    void paintEvent(QPaintEvent *event) override;

    void OnSetNewPath(const QString &newPath);
};
}
