//
// Created by gorev on 14.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_material_inspector_content.h" resolved

#include "material_inspector_content.h"

#include "Editor.h"
#include "Engine.h"
#include "FileSystemUtils.h"
#include "LabelsUtils.h"

#include "qfileinfo.h"
#include "input-widgets/path_input_field.h"
#include "oclero/qlementine/widgets/Label.hpp"

#include <qdir.h>

namespace editor
{
material_inspector_content::material_inspector_content(const QString &file, QWidget *parent)
    : browser_item_inspector_content(file, parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);

    YAML::Node node = YAML::LoadFile(ToString(file));

    m_nameLabel = new QLabel(ToHeader2(QFileInfo(file).completeBaseName()), this);
    m_nameLabel->setTextFormat(Qt::MarkdownText);
    mainLayout->addWidget(m_nameLabel);

    auto separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(separator);

    m_shaderField = new path_input_field("Shader", {formats::shaderFormat}, this);
    m_albedoField = new path_input_field("Albedo", formats::supportedTextureFormats, this);
    m_normalField = new path_input_field("Normal", formats::supportedTextureFormats, this);
    m_metallicField = new path_input_field("Metallic", formats::supportedTextureFormats, this);
    m_roughnessField = new path_input_field("Roughness", formats::supportedTextureFormats, this);
    m_aoField = new path_input_field("AO", formats::supportedTextureFormats, this);

    mainLayout->addWidget(m_shaderField);
    mainLayout->addWidget(m_albedoField);
    mainLayout->addWidget(m_normalField);
    mainLayout->addWidget(m_metallicField);
    mainLayout->addWidget(m_roughnessField);
    mainLayout->addWidget(m_aoField);

    auto setContentPath = [](path_input_field *field, const std::string &yamlPath)
    {
        if (yamlPath.empty())
        {
            field->SetPath("");
        }
        else
        {
            field->SetPath(QString::fromStdString(yamlPath));
        }
    };

    setContentPath(m_shaderField, node["ShaderPath"].as<std::string>(""));
    setContentPath(m_albedoField, node["AlbedoPath"].as<std::string>(""));
    setContentPath(m_normalField, node["NormalPath"].as<std::string>(""));
    setContentPath(m_metallicField, node["MetallicPath"].as<std::string>(""));
    setContentPath(m_roughnessField, node["RoughnessPath"].as<std::string>(""));
    setContentPath(m_aoField, node["AOPath"].as<std::string>(""));

    connectPathField(m_shaderField, "ShaderPath");
    connectPathField(m_albedoField, "AlbedoPath");
    connectPathField(m_normalField, "NormalPath");
    connectPathField(m_metallicField, "MetallicPath");
    connectPathField(m_roughnessField, "RoughnessPath");
    connectPathField(m_aoField, "AOPath");

    mainLayout->addStretch();
}


void material_inspector_content::connectPathField(path_input_field *field, const QString &yamlKey)
{
    connect(field, &path_input_field::PathChanged, this, [this, yamlKey](const QString &, const QString &newPath)
            { SetValueYAML(ToString(m_file), yamlKey.toStdString(), ToString(newPath)); });
}

} // namespace editor
