//
// Created by gorev on 25.09.2025.
//

#pragma once


#include "FileSystemUtils.h"


#include <QDialog>


class QLabel;
class QVBoxLayout;
class QHBoxLayout;

namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class import_asset_dialog;
}
QT_END_NAMESPACE

class import_asset_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit import_asset_dialog(const ImportAssetInfo &info, QWidget *parent = nullptr);
    ~import_asset_dialog() override;


protected slots:
    virtual void OnConfirm();
    virtual void OnCancel();

protected:
    ImportAssetInfo m_info;

    QHBoxLayout *m_layout;
    QPushButton *m_btn_confirm;
    QPushButton *m_btn_cancel;

    void CreateConfirmButtons();

private:
    Ui::import_asset_dialog *ui;
};
} // namespace editor
