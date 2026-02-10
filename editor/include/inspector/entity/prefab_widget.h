//
// Created by gorev on 08.02.2026.
//

#pragma once
#include "component_widget.h"

namespace editor
{

class prefab_widget : public component_widget<Blainn::TransformComponent>
{
    Q_OBJECT

public:
    explicit prefab_widget(const Blainn::Entity &entity, QWidget *parent = nullptr);

    void DeleteComponent() override;

private:
    QPushButton *m_applyChanges = nullptr;
    QPushButton *m_revertChanges = nullptr;
};

} // namespace editor