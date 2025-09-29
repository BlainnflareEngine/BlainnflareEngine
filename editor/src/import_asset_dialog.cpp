//
// Created by gorev on 25.09.2025.
//

#include "import_asset_dialog.h"
#include "ui_import_asset_dialog.h"

namespace editor
{
import_asset_dialog::import_asset_dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::import_asset_dialog)
{
    ui->setupUi(this);
}


import_asset_dialog::~import_asset_dialog()
{
    delete ui;
}
} // namespace editor
