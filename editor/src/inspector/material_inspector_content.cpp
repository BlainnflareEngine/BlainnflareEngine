//
// Created by gorev on 14.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_material_inspector_content.h" resolved

#include "material_inspector_content.h"

#include "AssetManager.h"
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
    BLAINN_PROFILE_FUNC();

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
    m_albedoColor = new color_input_field("Albedo color", {QColor(Qt::black)}, this);

    m_normalField = new path_input_field("Normal", formats::supportedTextureFormats, this);
    m_normalScale = new float_input_field("Normal scale", 0, this);
    m_normalScale->SetMaxValue(1);
    m_normalScale->SetMinValue(0);

    m_metallicField = new path_input_field("Metallic", formats::supportedTextureFormats, this);
    m_metallicScale = new float_input_field("Metallic scale", 0, this);
    m_metallicScale->SetMaxValue(1);
    m_metallicScale->SetMinValue(0);

    m_roughnessField = new path_input_field("Roughness", formats::supportedTextureFormats, this);
    m_roughnessScale = new float_input_field("Roughness scale", 0, this);
    m_roughnessScale->SetMaxValue(1);
    m_roughnessScale->SetMinValue(0);

    m_aoField = new path_input_field("AO", formats::supportedTextureFormats, this);

    mainLayout->addWidget(m_shaderField);

    mainLayout->addWidget(m_albedoField);
    mainLayout->addWidget(m_albedoColor);

    mainLayout->addWidget(m_normalField);
    mainLayout->addWidget(m_normalScale);

    mainLayout->addWidget(m_metallicField);
    mainLayout->addWidget(m_metallicScale);

    mainLayout->addWidget(m_roughnessField);
    mainLayout->addWidget(m_roughnessScale);

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

    if (node["AlbedoColor"]) m_albedoColor->SetValue(QColor::fromString(node["AlbedoColor"].as<std::string>("")));

    if (node["NormalScale"]) m_normalScale->SetValue(node["NormalScale"].as<float>());

    if (node["MetallicScale"]) m_metallicScale->SetValue(node["MetallicScale"].as<float>());

    if (node["RoughnessScale"]) m_roughnessScale->SetValue(node["RoughnessScale"].as<float>());

    connectPathField(m_shaderField, "ShaderPath");
    connectPathField(m_albedoField, "AlbedoPath");
    connectPathField(m_normalField, "NormalPath");
    connectPathField(m_metallicField, "MetallicPath");
    connectPathField(m_roughnessField, "RoughnessPath");
    connectPathField(m_aoField, "AOPath");

    connectFloatField(m_normalScale, "NormalScale");
    connectFloatField(m_metallicScale, "MetallicScale");
    connectFloatField(m_roughnessScale, "RoughnessScale");

    connect(m_albedoColor, &color_input_field::EditingFinished, this,
            [this]()
            {
                QDir content(QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));
                SetValueYAML(ToString(m_file), "AlbedoColor",
                             ToString(m_albedoColor->GetValue().name(QColor::HexArgb)));
                Blainn::AssetManager::GetInstance().UpdateMaterial(ToString(content.relativeFilePath(m_file)));
            });

    mainLayout->addStretch();
}


void material_inspector_content::connectPathField(path_input_field *field, const QString &yamlKey)
{
    connect(field, &path_input_field::PathChanged, this,
            [this, yamlKey](const QString &, const QString &newPath)
            {
                QDir content(QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));
                auto relPath = content.relativeFilePath(newPath);
                SetValueYAML(ToString(m_file), yamlKey.toStdString(), ToString(newPath));
                Blainn::AssetManager::GetInstance().UpdateMaterial(ToString(content.relativeFilePath(m_file)));
            });
}

void material_inspector_content::connectFloatField(float_input_field *field, const QString &yamlKey)
{
    connect(field, &float_input_field::EditingFinished, this,
            [this, yamlKey, field]()
            {
                QDir content(QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));
                SetValueYAML(ToString(m_file), yamlKey.toStdString(), std::to_string(field->GetValue()));
                Blainn::AssetManager::GetInstance().UpdateMaterial(ToString(content.relativeFilePath(m_file)));
            });
}

} // namespace editor
