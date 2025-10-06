//
// Created by gorev on 02.10.2025.
//

#ifndef IMPORT_MODEL_DIALOG_H
#define IMPORT_MODEL_DIALOG_H

#include "dialog/import_asset_dialog.h"

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

protected:
    void OnConfirm() override;
    void OnCancel() override;

private:
    Ui::import_model_dialog *ui;
};
} // namespace editor

#endif // IMPORT_MODEL_DIALOG_H
