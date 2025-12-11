//
// Created by gorev on 06.10.2025.
//

#ifndef CREATE_MATERIAL_DIALOG_H
#define CREATE_MATERIAL_DIALOG_H


#include "input-widgets/color_input_field.h"


#include <QDialog>


namespace editor
{
class float_input_field;
}
class QLineEdit;
class QLabel;
namespace editor
{
class path_input_field;

class create_material_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit create_material_dialog(QWidget *parent = nullptr);

    QString GetMaterialName() const;
    QString GetAlbedoPath() const;
    QColor GetAlbedoColor() const;
    QString GetNormalPath() const;
    float GetNormalScale() const;
    QString GetMetallicPath() const;
    float GetMetallicScale() const;
    QString GetRoughnessPath() const;
    float GetRoughnessScale() const;
    QString GetAOPath() const;
    QString GetShaderPath() const;


public slots:
    void accept() override;

private:
    QLineEdit *m_materialName;
    path_input_field *m_shaderPath;
    path_input_field *m_albedoPath;
    color_input_field *m_albedoColor;
    path_input_field *m_normalPath;
    float_input_field *m_normalScale;
    path_input_field *m_metallicPath;
    float_input_field *m_metallicScale;
    path_input_field *m_roughnessPath;
    float_input_field *m_roughnessScale;
    path_input_field *m_aoPath;

    QPushButton *m_acceptButton;
    QPushButton *m_cancelButton;
};

} // namespace editor

#endif // CREATE_MATERIAL_DIALOG_H
