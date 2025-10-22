//
// Created by gorev on 22.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_mesh_inspector_content.h" resolved

#include "mesh_inspector_content.h"

#include "FileSystemUtils.h"
#include "LabelsUtils.h"
#include "ui_mesh_inspector_content.h"

#include <QDir>
#include <qfileinfo.h>

namespace editor
{
mesh_inspector_content::mesh_inspector_content(const QString &file, QWidget *parent)
    : browser_item_inspector_content(file, parent)
    , ui(new Ui::mesh_inspector_content)
{
    ui->setupUi(this);

    QFileInfo fileInfo = QFileInfo(file);
    QString metaFile = fileInfo.absoluteFilePath().append(".blainn");

    YAML::Node meta = YAML::LoadFile(ToString(metaFile));
    if (meta.IsDefined())
    {
        ui->Name->setText(ToHeader2(fileInfo.fileName()));
        ui->ConvertToLH->setChecked(meta["ConvertToLH"].as<bool>());
    }
    else
    {
        BF_ERROR("No meta file found for {0}", ToString(QFileInfo(file).fileName()));
    }
}

mesh_inspector_content::~mesh_inspector_content()
{
    delete ui;
}
} // namespace editor
