//
// Created by gorev on 14.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_inspector_content_base.h" resolved

#include "inspector_content_base.h"
#include "ui_inspector_content_base.h"

namespace editor
{
inspector_content_base::inspector_content_base(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::inspector_content_base)
{
    ui->setupUi(this);
}

inspector_content_base::~inspector_content_base()
{
    delete ui;
}
} // namespace editor
