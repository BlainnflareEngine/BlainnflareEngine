//
// Created by gorev on 21.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_import_texture_dialog.h" resolved

#include "import_texture_dialog.h"
#include "ui_import_texture_dialog.h"

namespace editor
{
import_texture_dialog::import_texture_dialog(const ImportAssetInfo &info, QWidget *parent)
    : import_asset_dialog(info, parent)
    , ui(new Ui::import_texture_dialog)
{
    ui->setupUi(this);
}

import_texture_dialog::~import_texture_dialog()
{
    delete ui;
}
} // namespace editor
