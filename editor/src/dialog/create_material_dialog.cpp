//
// Created by gorev on 06.10.2025.
//


#include "dialog/create_material_dialog.h"

#include "FileSystemUtils.h"
#include "LabelsUtils.h"
#include "input-widgets/float_input_field.h"
#include "input-widgets/path_input_field.h"
#include "oclero/qlementine/widgets/Label.hpp"
#include "oclero/qlementine/widgets/LineEdit.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <qfileinfo.h>

namespace editor
{
create_material_dialog::create_material_dialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Create New Material");
    setModal(true);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    auto nameLabel = new QLabel(ToHeader3("Material Name:"), this);
    nameLabel->setTextFormat(Qt::MarkdownText);

    m_materialName = new QLineEdit(this);
    m_materialName->setPlaceholderText("Enter material name...");
    m_materialName->setFixedHeight(30);
    m_materialName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(m_materialName);

    m_shaderPath = new path_input_field("Shader", {formats::shaderFormat}, this);

    m_albedoPath = new path_input_field("Albedo", formats::supportedTextureFormats, this);
    m_albedoColor = new color_input_field("Albedo color", QColor(255, 255, 255, 255), this);

    m_normalPath = new path_input_field("Normal", formats::supportedTextureFormats, this);
    m_normalScale = new float_input_field("Normal scale", 1.0f, this);
    m_normalScale->SetMinValue(0.0f);
    m_normalScale->SetMaxValue(1.0f);

    m_metallicPath = new path_input_field("Metallic", formats::supportedTextureFormats, this);
    m_metallicScale = new float_input_field("Metallic scale", 1.0f, this);
    m_metallicScale->SetMinValue(0.0f);
    m_metallicScale->SetMaxValue(1.0f);

    m_roughnessPath = new path_input_field("Roughness", formats::supportedTextureFormats, this);
    m_roughnessScale = new float_input_field("Roughness scale", 1.0f, this);
    m_roughnessScale->SetMinValue(0.0f);
    m_roughnessScale->SetMaxValue(1.0f);

    m_aoPath = new path_input_field("AO", formats::supportedTextureFormats, this);

    mainLayout->addWidget(m_shaderPath);

    mainLayout->addWidget(m_albedoPath);
    mainLayout->addWidget(m_albedoColor);

    mainLayout->addWidget(m_normalPath);
    mainLayout->addWidget(m_normalScale);

    mainLayout->addWidget(m_metallicPath);
    mainLayout->addWidget(m_metallicScale);

    mainLayout->addWidget(m_roughnessPath);
    mainLayout->addWidget(m_roughnessScale);

    mainLayout->addWidget(m_aoPath);

    auto *buttonLayout = new QHBoxLayout();
    m_acceptButton = new QPushButton("Create", this);
    m_cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_acceptButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(m_acceptButton, &QPushButton::clicked, this, &create_material_dialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &create_material_dialog::reject);
}


QString create_material_dialog::GetMaterialName() const
{
    return m_materialName->text().trimmed();
}


QString create_material_dialog::GetAlbedoPath() const
{
    QString path = m_albedoPath->GetAbsolutePath();
    if (!path.isEmpty() && QFileInfo::exists(path)) return path;
    return QString();
}


QColor create_material_dialog::GetAlbedoColor() const
{
    return m_albedoColor->GetValue();
}


QString create_material_dialog::GetNormalPath() const
{
    QString path = m_normalPath->GetAbsolutePath();
    if (!path.isEmpty() && QFileInfo::exists(path)) return path;
    return QString();
}


float create_material_dialog::GetNormalScale() const
{
    return m_normalScale->GetValue();
}


QString create_material_dialog::GetMetallicPath() const
{
    QString path = m_metallicPath->GetAbsolutePath();
    if (!path.isEmpty() && QFileInfo::exists(path)) return path;
    return QString();
}


float create_material_dialog::GetMetallicScale() const
{
    return m_metallicScale->GetValue();
}


QString create_material_dialog::GetRoughnessPath() const
{
    QString path = m_roughnessPath->GetAbsolutePath();
    if (!path.isEmpty() && QFileInfo::exists(path)) return path;
    return QString();
}


float create_material_dialog::GetRoughnessScale() const
{
    return m_roughnessScale->GetValue();
}


QString create_material_dialog::GetAOPath() const
{
    QString path = m_aoPath->GetAbsolutePath();
    if (!path.isEmpty() && QFileInfo::exists(path)) return path;
    return QString();
}


QString create_material_dialog::GetShaderPath() const
{
    QString path = m_shaderPath->GetAbsolutePath();
    if (!path.isEmpty() && QFileInfo::exists(path)) return path;
    return QString();
}

void create_material_dialog::accept()
{
    if (GetMaterialName().isEmpty())
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Error", "Material name is empty!");
        messageBox.setFixedSize(300, 200);
        return;
    }

    QDialog::accept();
}
} // namespace editor
