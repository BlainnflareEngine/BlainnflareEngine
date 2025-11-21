//
// Created by gorev on 02.10.2025.
//

#pragma once

#include "../../../common/ImportAssetData.h"
#include "dialog/import_asset_dialog.h"

class QCheckBox;
namespace editor
{

QT_BEGIN_NAMESPACE
namespace Ui
{
class import_model_dialog;
}
QT_END_NAMESPACE

class import_model_dialog : public import_asset_dialog
{
    Q_OBJECT

public:
    explicit import_model_dialog(const ImportAssetInfo &info, QWidget *parent = nullptr);
    ~import_model_dialog() override;

    //Blainn::ImportMeshData &GetData();

protected:
    void OnConfirm() override;
    void OnCancel() override;

private:
    Ui::import_model_dialog *ui;

    Blainn::ImportMeshData m_importData;
};
} // namespace editor
