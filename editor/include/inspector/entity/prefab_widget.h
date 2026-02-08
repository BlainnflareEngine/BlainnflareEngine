//
// Created by gorev on 08.02.2026.
//

#pragma once
#include "component_widget_base.h"

namespace editor
{

class prefab_widget : public component_widget_base
{
    Q_OBJECT

public:
    explicit prefab_widget(const Blainn::Entity& entity,QWidget *parent = nullptr);

    void DeleteComponent() override;
};

} // namespace editor