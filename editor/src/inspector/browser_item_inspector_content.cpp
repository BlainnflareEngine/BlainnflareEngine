//
// Created by gorev on 14.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_material_inspector_content.h" resolved

#include "browser_item_inspector_content.h"
#include "ui_browser_item_inspector_content.h"

namespace editor
{
browser_item_inspector_content::browser_item_inspector_content(const QString &file, QWidget *parent)
    : inspector_content_base(parent)
    , ui(new Ui::material_inspector_content)
    , m_file(file)
{
    ui->setupUi(this);
}

browser_item_inspector_content::~browser_item_inspector_content()
{
    delete ui;
}
} // namespace editor
