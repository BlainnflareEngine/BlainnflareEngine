//
// Created by gorev on 14.10.2025.
//

#pragma once


#include "browser_item_inspector_content.h"
#include "input-widgets/color_input_field.h"
#include "input-widgets/float_input_field.h"


class QLabel;
class QPushButton;
namespace editor
{
class path_input_field;

class material_inspector_content : public browser_item_inspector_content
{
    Q_OBJECT

public:
    explicit material_inspector_content(const QString &file, QWidget *parent = nullptr);

private:
    void connectPathField(path_input_field *field, const QString &yamlKey);
    void connectFloatField(float_input_field* field, const QString &yamlKey);

    QLabel *m_nameLabel = nullptr;

    path_input_field *m_shaderField = nullptr;
    path_input_field *m_albedoField = nullptr;
    color_input_field *m_albedoColor = nullptr;
    path_input_field *m_normalField = nullptr;
    float_input_field *m_normalScale = nullptr;
    path_input_field *m_metallicField = nullptr;
    float_input_field *m_metallicScale = nullptr;
    path_input_field *m_roughnessField = nullptr;
    float_input_field *m_roughnessScale = nullptr;
    path_input_field *m_aoField = nullptr;
};
} // namespace editor
