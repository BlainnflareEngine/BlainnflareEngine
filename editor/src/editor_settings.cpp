//
// Created by gorev on 28.09.2025.
//


#include "editor_settings.h"

#include "Editor.h"
#include "Engine.h"
#include "FileSystemUtils.h"
#include "ui_editor_settings.h"
#include "input-widgets/path_input_field.h"

#include <QLabel>
#include <QFileDialog>

namespace editor
{

editor_settings::editor_settings(const SettingsData &data, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::editor_settings)
    , data(data)
{
    ui->setupUi(this);

    auto defaultSceneLabel = new QLabel("Default Scene:", this);
    defaultSceneLabel->setStyleSheet("font-weight: bold;");

    m_defaultSceneInput = new path_input_field("Scene", {formats::sceneFormat}, this);
    m_defaultSceneInput->SetPath(QString::fromStdString(Blainn::Engine::GetConfig().GetDefaultSceneName().string()));

    if (auto settingsLayout = qobject_cast<QVBoxLayout *>(ui->SettingsArea->layout()))
    {
        settingsLayout->addWidget(defaultSceneLabel);
        settingsLayout->addWidget(m_defaultSceneInput);
    }

    connect(m_defaultSceneInput, &path_input_field::PathChanged, this,
            [this](const QString &, const QString &newPath) { OnDefaultScenePathChanged(newPath); });

    // ui->SettingsArea->layout()->addWidget(ui->ContentFolderPath);
}


editor_settings::~editor_settings()
{
    delete ui;
}


void editor_settings::OnDefaultScenePathChanged(const QString &newPath)
{
    Blainn::Engine::GetConfig().SetDefaultScene(ToEASTLString(newPath));
    Blainn::Engine::GetConfig().SaveConfig();
}
} // namespace editor
