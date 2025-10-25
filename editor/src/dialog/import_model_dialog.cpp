//
// Created by gorev on 02.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_import_model_dialog.h" resolved

#include "dialog/import_model_dialog.h"

#include "AssetManager.h"
#include "Editor.h"
#include "random.h"
#include "ui_import_model_dialog.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <fstream>
#include <qdir.h>
#include <qfileinfo.h>

namespace editor
{
import_model_dialog::import_model_dialog(const ImportAssetInfo &info, QWidget *parent)
    : import_asset_dialog(info, parent)
    , ui(new Ui::import_model_dialog)
{
    ui->setupUi(this);

    ui->Path->setText(info.originalPath);
    ui->ConvertToLH->setChecked(true);
    ui->CreateMaterials->setChecked(true);

    connect(ui->ConfirmButton, &QPushButton::clicked, this, &import_model_dialog::OnConfirm);
    connect(ui->CancelButton, &QPushButton::clicked, this, &import_model_dialog::OnCancel);
}


import_model_dialog::~import_model_dialog()
{
    delete ui;
}


Blainn::ImportMeshData &import_model_dialog::GetData()
{
    return m_importData;
}


void import_model_dialog::OnConfirm()
{
    QDir dir(Blainn::Editor::GetInstance().GetContentDirectory());
    YAML::Node meta;
    meta["ID"] = Blainn::Rand::getRandomUUID().str();
    meta["ModelPath"] = ToString(dir.relativeFilePath(m_info.destinationPath));
    meta["ConvertToLH"] = m_importData.convertToLH;
    meta["CreateMaterials"] = m_importData.createMaterials;

    QFileInfo fileInfo(m_info.originalPath);
    Blainn::Path modelPath = Blainn::Path(ToString(m_info.destinationPath));
    Blainn::Path configFilePath = modelPath.concat(".").concat(ToString(metaFormat));
    std::ofstream fout(configFilePath.string());
    fout << meta;

    import_asset_dialog::OnConfirm();
}


void import_model_dialog::OnCancel()
{
    import_asset_dialog::OnCancel();
}
} // namespace editor
