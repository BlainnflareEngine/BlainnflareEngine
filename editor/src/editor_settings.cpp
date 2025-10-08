//
// Created by gorev on 28.09.2025.
//


#include "editor_settings.h"

#include "Editor.h"
#include "ui_editor_settings.h"

#include <QFileDialog>

namespace editor
{

editor_settings::editor_settings(const SettingsData &data, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::editor_settings)
    , data(data)
{
    ui->setupUi(this);

    ui->ContentFolderPath->setText(QString::fromStdString(data.m_contentDirectory.string()));

    connect(ui->PickContentButton, &QPushButton::clicked, this, &editor_settings::OnSetDirectoryPressed);
}


editor_settings::~editor_settings()
{
    delete ui;
}


void editor_settings::OnSetDirectoryPressed()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Set directory"), "",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);


    if (dir.isEmpty()) return;

    ui->ContentFolderPath->setText(dir);
    Blainn::Editor::GetInstance().SetContentDirectory(dir.toStdString());
}
} // namespace editor
