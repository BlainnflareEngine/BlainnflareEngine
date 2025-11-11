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

    connect(ui->BrowseShader, &QPushButton::clicked, this,
            [&]()
            {
                SelectFile(*ui->Shader, filters::ShaderFilter,
                           QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));
                QDir dir(Blainn::Engine::GetContentDirectory());
                SetValueYAML(ToString(m_file), "ShaderPath", ToString(dir.relativeFilePath(ui->Shader->text())));
            });

    connect(ui->BrowseAlbedo, &QPushButton::clicked, this,
            [&]()
            {
                SelectFile(*ui->Albedo, filters::TextureFilter,
                           QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));
                QDir dir(Blainn::Engine::GetContentDirectory());
                SetValueYAML(ToString(m_file), "AlbedoPath", ToString(dir.relativeFilePath(ui->Albedo->text())));
            });

    connect(ui->BrowseNormal, &QPushButton::clicked, this,
            [&]()
            {
                SelectFile(*ui->Normal, filters::TextureFilter,
                           QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));
                QDir dir(Blainn::Engine::GetContentDirectory());
                SetValueYAML(ToString(m_file), "NormalPath", ToString(dir.relativeFilePath(ui->Normal->text())));
            });

    connect(ui->BrowseMetallic, &QPushButton::clicked, this,
            [&]()
            {
                SelectFile(*ui->Metallic, filters::TextureFilter,
                           QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));
                QDir dir(Blainn::Engine::GetContentDirectory());
                SetValueYAML(ToString(m_file), "MetallicPath", ToString(dir.relativeFilePath(ui->Metallic->text())));
            });

    connect(ui->BrowseRoughness, &QPushButton::clicked, this,
            [&]()
            {
                SelectFile(*ui->Roughness, filters::TextureFilter,
                           QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));
                QDir dir(Blainn::Engine::GetContentDirectory());
                SetValueYAML(ToString(m_file), "RoughnessPath", ToString(dir.relativeFilePath(ui->Roughness->text())));
            });

    connect(ui->BrowseAO, &QPushButton::clicked, this,
            [&]()
            {
                SelectFile(*ui->AO, filters::TextureFilter,
                           QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));
                QDir dir(Blainn::Engine::GetContentDirectory());
                SetValueYAML(ToString(m_file), "AOPath", ToString(dir.relativeFilePath(ui->AO->text())));
            });
}

material_inspector_content::~material_inspector_content()
{
    delete ui;
}


QString material_inspector_content::GetPathString(const std::string &path)
{
    if (path.empty()) return "No texture";

    return QString::fromStdString(path);
}
} // namespace editor
