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
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
    dialog->setOption(QFileDialog::DontResolveSymlinks, true);
    dialog->setWindowTitle(tr("Set directory"));

    QString currentDir = ui->ContentFolderPath->text();
    if (!currentDir.isEmpty() && QDir(currentDir).exists()) dialog->setDirectory(currentDir);

    connect(dialog, &QFileDialog::finished, this,
            [this, dialog](int result)
            {
                if (result == Accepted && !dialog->selectedFiles().isEmpty())
                {
                    QString dir = dialog->selectedFiles().first();
                    ui->ContentFolderPath->setText(dir);
                    Blainn::Editor::GetInstance().SetContentDirectory(dir.toStdString());
                }

                dialog->deleteLater();
            });

    dialog->open();
}
} // namespace editor
