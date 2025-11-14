//
// Created by gorev on 14.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_material_inspector_content.h" resolved

#include "material_inspector_content.h"

#include "Editor.h"
#include "Engine.h"
#include "FileSystemUtils.h"
#include "LabelsUtils.h"
#include "ui_material_inspector_content.h"

#include "qfileinfo.h"
#include <qdir.h>

namespace editor
{
material_inspector_content::material_inspector_content(const QString &file, QWidget *parent)
    : browser_item_inspector_content(file, parent)
    , ui(new Ui::material_inspector_content)
{
    ui->setupUi(this);

    YAML::Node node = YAML::LoadFile(ToString(file));
    ui->Name->setTextFormat(Qt::MarkdownText);
    ui->Name->setText(ToHeader2(QFileInfo(file).completeBaseName()));

    ui->Shader->setText(GetPathString(node["ShaderPath"].as<std::string>()));
    ui->Albedo->setText(GetPathString(node["AlbedoPath"].as<std::string>()));
    ui->Normal->setText(GetPathString(node["NormalPath"].as<std::string>()));
    ui->Metallic->setText(GetPathString(node["MetallicPath"].as<std::string>()));
    ui->Roughness->setText(GetPathString(node["RoughnessPath"].as<std::string>()));
    ui->AO->setText(GetPathString(node["AOPath"].as<std::string>()));

    setupFileBrowse(ui->BrowseShader, ui->Shader, "ShaderPath", "Select Shader", filters::ShaderFilter);
    setupFileBrowse(ui->BrowseAlbedo, ui->Albedo, "AlbedoPath", "Select Albedo Texture", filters::TextureFilter);
    setupFileBrowse(ui->BrowseNormal, ui->Normal, "NormalPath", "Select Normal Map", filters::TextureFilter);
    setupFileBrowse(ui->BrowseMetallic, ui->Metallic, "MetallicPath", "Select Metallic Map", filters::TextureFilter);
    setupFileBrowse(ui->BrowseRoughness, ui->Roughness, "RoughnessPath", "Select Roughness Map",
                    filters::TextureFilter);
    setupFileBrowse(ui->BrowseAO, ui->AO, "AOPath", "Select Ambient Occlusion Map", filters::TextureFilter);
}

material_inspector_content::~material_inspector_content()
{
    delete ui;
}


QString material_inspector_content::GetPathString(const std::string &path)
{
    if (path.empty()) return "No resource";

    return QString::fromStdString(path);
}


void material_inspector_content::setupFileBrowse(QPushButton *button, QLabel *label, const QString &yamlKey,
                                                 const QString &dialogTitle, const QString &filter)
{
    connect(button, &QPushButton::clicked, this,
            [this, label, yamlKey, dialogTitle, filter]()
            {
                QString currentText = label->text();
                QString initialDir = currentText == "No resource" || currentText.isEmpty()
                                         ? QString::fromStdString(Blainn::Engine::GetContentDirectory().string())
                                         : QFileInfo(currentText).absolutePath();

                SelectFileAsync(this, dialogTitle, initialDir, filter,
                                [this, label, yamlKey](const QString &selectedFile)
                                {
                                    QDir contentDir(Blainn::Engine::GetContentDirectory());
                                    QString relPath = contentDir.relativeFilePath(selectedFile);

                                    label->setText(relPath);
                                    SetValueYAML(ToString(m_file), yamlKey.toStdString(), ToString(relPath));
                                });
            });
}
} // namespace editor
