//
// Created by gorev on 06.10.2025.
//

#ifndef CREATE_MATERIAL_DIALOG_H
#define CREATE_MATERIAL_DIALOG_H

#include <QDialog>


class QLabel;
namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class create_material_dialog;
}
QT_END_NAMESPACE

class create_material_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit create_material_dialog(QWidget *parent = nullptr);
    ~create_material_dialog() override;

    QString GetMaterialName() const;
    QString GetAlbedoPath() const;
    QString GetNormalPath() const;
    QString GetMetallicPath() const;
    QString GetRoughnessPath() const;
    QString GetAOPath() const;
    QString GetShaderPath() const;


public slots:
    void OnSelectAlbedo();
    void OnSelectNormal();
    void OnSelectMetallic();
    void OnSelectRoughness();
    void OnSelectAO();
    void OnSelectShader();
    void accept() override;

private:
    Ui::create_material_dialog *ui;

    void SelectFile(QLabel &label, const QString &filter);
};

} // namespace editor

#endif // CREATE_MATERIAL_DIALOG_H
