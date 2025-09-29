//
// Created by gorev on 25.09.2025.
//

#ifndef IMPORT_ASSET_DIALOG_H
#define IMPORT_ASSET_DIALOG_H

#include "EASTL/unique_ptr.h"


#include <QDialog>
#include <QPointer>

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
    explicit import_asset_dialog(QWidget *parent = nullptr);
    ~import_asset_dialog() override;

private:
    Ui::import_asset_dialog* ui;
};
} // namespace editor

#endif // IMPORT_ASSET_DIALOG_H
