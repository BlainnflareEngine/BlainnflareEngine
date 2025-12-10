//
// Created by gorev on 06.10.2025.
//

#ifndef CREATE_MATERIAL_DIALOG_H
#define CREATE_MATERIAL_DIALOG_H


#include <QDialog>


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
    QString GetNormalPath() const;
    QString GetMetallicPath() const;
    QString GetRoughnessPath() const;
    QString GetAOPath() const;
    QString GetShaderPath() const;


public slots:
    void accept() override;

private:
    QLineEdit *m_materialName;
    path_input_field *m_shaderPath;
    path_input_field *m_albedoPath;
    path_input_field *m_normalPath;
    path_input_field *m_metallicPath;
    path_input_field *m_roughnessPath;
    path_input_field *m_aoPath;

    QPushButton *m_acceptButton;
    QPushButton *m_cancelButton;
};

} // namespace editor

#endif // CREATE_MATERIAL_DIALOG_H
