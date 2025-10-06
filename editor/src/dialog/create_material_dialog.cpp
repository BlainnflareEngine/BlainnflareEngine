//
// Created by gorev on 06.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_create_material_dialog.h" resolved

#include "dialog/create_material_dialog.h"

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

    connect(ui->AlbedoBrowse, &QPushButton::pressed, this, &create_material_dialog::OnSelectAlbedo);
    connect(ui->NormalBrowse, &QPushButton::pressed, this, &create_material_dialog::OnSelectNormal);
    connect(ui->MetallicBrowse, &QPushButton::pressed, this, &create_material_dialog::OnSelectMetallic);
    connect(ui->RoughnessBrowse, &QPushButton::pressed, this, &create_material_dialog::OnSelectRoughness);
    connect(ui->AOBrowse, &QPushButton::pressed, this, &create_material_dialog::OnSelectAO);

    connect(ui->Accept, &QPushButton::pressed, this, &create_material_dialog::accept);
    connect(ui->Cancel, &QPushButton::pressed, this, &create_material_dialog::reject);
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


void create_material_dialog::OnSelectAlbedo()
{
    SelectTextureFile(*ui->AlbedoPath);
}


void create_material_dialog::OnSelectNormal()
{
    SelectTextureFile(*ui->NormaPath);
}


void create_material_dialog::OnSelectMetallic()
{
    SelectTextureFile(*ui->MetallicPath);
}


void create_material_dialog::OnSelectRoughness()
{
    SelectTextureFile(*ui->RoughnessPath);
}


void create_material_dialog::OnSelectAO()
{
    SelectTextureFile(*ui->AOPath);
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


void create_material_dialog::SelectTextureFile(QLabel &label)
{
    QString filter = "Textures (*.png *.dds *.jpg *.jpeg *.tga *.bmp);;All Files (*)";

    QString fileName = QFileDialog::getOpenFileName(
        this, "Select Texture File", label.text().isEmpty() ? "." : QFileInfo(label.text()).absolutePath(), filter);

    if (!fileName.isEmpty())
    {
        label.setText(fileName);
    }
}
} // namespace editor
