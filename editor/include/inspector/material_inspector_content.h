//
// Created by gorev on 14.10.2025.
//

#pragma once


#include "browser_item_inspector_content.h"

namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class material_inspector_content;
}
QT_END_NAMESPACE

class material_inspector_content : public browser_item_inspector_content
{
    Q_OBJECT

public:
    explicit material_inspector_content(const QString &file, QWidget *parent = nullptr);
    ~material_inspector_content() override;

private:
    Ui::material_inspector_content *ui;

    static QString GetPathString(const std::string &path);
};
} // namespace editor
