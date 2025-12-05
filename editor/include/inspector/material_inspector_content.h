//
// Created by gorev on 14.10.2025.
//

#pragma once


#include "browser_item_inspector_content.h"


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

    QLabel *m_nameLabel = nullptr;

    path_input_field *m_shaderField = nullptr;
    path_input_field *m_albedoField = nullptr;
    path_input_field *m_normalField = nullptr;
    path_input_field *m_metallicField = nullptr;
    path_input_field *m_roughnessField = nullptr;
    path_input_field *m_aoField = nullptr;
};
} // namespace editor
