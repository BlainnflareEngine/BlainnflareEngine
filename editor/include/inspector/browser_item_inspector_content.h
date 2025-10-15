//
// Created by gorev on 14.10.2025.
//

#pragma once


#include "inspector_content_base.h"

namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class material_inspector_content;
}
QT_END_NAMESPACE

class browser_item_inspector_content : public inspector_content_base
{
    Q_OBJECT

public:
    explicit browser_item_inspector_content(const QString &file, QWidget *parent = nullptr);
    ~browser_item_inspector_content() override;

protected:
    QString m_file;

private:
    Ui::material_inspector_content *ui;
};
} // namespace editor
