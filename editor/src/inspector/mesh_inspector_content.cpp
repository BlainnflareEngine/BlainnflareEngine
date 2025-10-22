//
// Created by gorev on 22.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mesh_inspector_content.h" resolved

#include "mesh_inspector_content.h"
#include "ui_mesh_inspector_content.h"

namespace editor
{
mesh_inspector_content::mesh_inspector_content(const QString &file, QWidget *parent)
    : browser_item_inspector_content(file, parent)
    , ui(new Ui::mesh_inspector_content)
{
    ui->setupUi(this);
}

mesh_inspector_content::~mesh_inspector_content()
{
    delete ui;
}
} // namespace editor
