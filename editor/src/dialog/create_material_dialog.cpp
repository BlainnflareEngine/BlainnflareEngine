//
// Created by gorev on 06.10.2025.
//


#include "dialog/create_material_dialog.h"

#include "FileSystemUtils.h"
#include "ui_create_material_dialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <qfileinfo.h>

namespace editor
{
create_material_dialog::create_material_dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::create_material_dialog)
{
    ui->setupUi(this);

    connect(ui->AlbedoBrowse, &QPushButton::clicked, this, &create_material_dialog::OnSelectAlbedo);
    connect(ui->NormalBrowse, &QPushButton::clicked, this, &create_material_dialog::OnSelectNormal);
    connect(ui->MetallicBrowse, &QPushButton::clicked, this, &create_material_dialog::OnSelectMetallic);
    connect(ui->RoughnessBrowse, &QPushButton::clicked, this, &create_material_dialog::OnSelectRoughness);
    connect(ui->AOBrowse, &QPushButton::clicked, this, &create_material_dialog::OnSelectAO);
    connect(ui->ShaderBrowse, &QPushButton::clicked, this, &create_material_dialog::OnSelectShader);

    connect(ui->Accept, &QPushButton::clicked, this, &create_material_dialog::accept);
    connect(ui->Cancel, &QPushButton::clicked, this, &create_material_dialog::reject);
}


create_material_dialog::~create_material_dialog()
{
    delete ui;
}


QString create_material_dialog::GetMaterialName() const
{
    return ui->MaterialName->text();
}


QString create_material_dialog::GetAlbedoPath() const
{
    if (QFileInfo(ui->AlbedoPath->text()).exists()) return ui->AlbedoPath->text();

    return QString();
}


QString create_material_dialog::GetNormalPath() const
{
    if (QFileInfo(ui->NormaPath->text()).exists()) return ui->NormaPath->text();

    return QString();
}


QString create_material_dialog::GetMetallicPath() const
{
    if (QFileInfo(ui->MetallicPath->text()).exists()) return ui->MetallicPath->text();

    return QString();
}


QString create_material_dialog::GetRoughnessPath() const
{
    if (QFileInfo(ui->RoughnessPath->text()).exists()) return ui->RoughnessPath->text();

    return QString();
}


QString create_material_dialog::GetAOPath() const
{
    if (QFileInfo(ui->AOPath->text()).exists()) return ui->AOPath->text();

    return QString();
}


QString create_material_dialog::GetShaderPath() const
{
    if (QFileInfo(ui->ShaderPath->text()).exists()) return ui->ShaderPath->text();

    // TODO: use default shader
    return QString();
}


void create_material_dialog::OnSelectAlbedo()
{
    SelectFile(*ui->AlbedoPath, filters::TextureFilter);
}


void create_material_dialog::OnSelectNormal()
{
    SelectFile(*ui->NormaPath, filters::TextureFilter);
}


void create_material_dialog::OnSelectMetallic()
{
    SelectFile(*ui->MetallicPath, filters::TextureFilter);
}


void create_material_dialog::OnSelectRoughness()
{
    SelectFile(*ui->RoughnessPath, filters::TextureFilter);
}


void create_material_dialog::OnSelectAO()
{
    SelectFile(*ui->AOPath, filters::TextureFilter);
}


void create_material_dialog::OnSelectShader()
{
    SelectFile(*ui->ShaderPath, filters::ShaderFilter);
}


void create_material_dialog::accept()
{
    if (ui->MaterialName->text().isEmpty())
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Error", "Material name is empty!");
        messageBox.setFixedSize(300, 200);
        return;
    }

    QDialog::accept();
}
} // namespace editor
