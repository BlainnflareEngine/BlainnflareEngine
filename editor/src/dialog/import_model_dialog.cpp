//
// Created by gorev on 02.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_import_model_dialog.h" resolved

#include "dialog/import_model_dialog.h"
#include "ui_import_model_dialog.h"

namespace editor
{
import_model_dialog::import_model_dialog(const ImportAssetInfo &info, QWidget *parent)
    : import_asset_dialog(info, parent)
    , ui(new Ui::import_model_dialog)
{
    ui->setupUi(this);
}


import_model_dialog::~import_model_dialog()
{
    delete ui;
}


void import_model_dialog::OnConfirm()
{
    BF_DEBUG("Add model to model list");
    import_asset_dialog::OnConfirm();
}


void import_model_dialog::OnCancel()
{
    import_asset_dialog::OnCancel();
}
} // namespace editor
