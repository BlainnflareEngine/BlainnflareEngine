//
// Created by gorev on 17.11.2025.
//

#pragma once
#include "component_widget_base.h"

namespace editor
{
class path_input_field;

class mesh_widget : public component_widget_base
{
public:
    explicit mesh_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

protected:
    void UpdatePath();
    void UpdateMaterial();

    void SetNewPath(const QString &oldPath, const QString &newPath);
    void SetNewMaterial(const QString &oldPath, const QString &newPath);
    virtual void DeleteComponent() override;
    void BlockSignals(bool block);

    path_input_field *m_path_input;
    path_input_field *m_material_input;
};

} // namespace editor