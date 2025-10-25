//
// Created by gorev on 21.10.2025.
//

#pragma once

#include "import_asset_dialog.h"

namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class import_texture_dialog;
}
QT_END_NAMESPACE

class import_texture_dialog : public import_asset_dialog
{
    Q_OBJECT

public:
    explicit import_texture_dialog(const ImportAssetInfo &info, QWidget *parent = nullptr);
    ~import_texture_dialog() override;

private:
    Ui::import_texture_dialog *ui;
};
} // namespace editor
