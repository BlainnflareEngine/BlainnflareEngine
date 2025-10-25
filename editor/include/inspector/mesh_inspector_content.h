//
// Created by gorev on 22.10.2025.
//

#ifndef MESH_INSPECTOR_CONTENT_H
#define MESH_INSPECTOR_CONTENT_H

#include "browser_item_inspector_content.h"

namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class mesh_inspector_content;
}
QT_END_NAMESPACE

class mesh_inspector_content : public browser_item_inspector_content
{
    Q_OBJECT

public:
    explicit mesh_inspector_content(const QString &file, QWidget *parent = nullptr);
    ~mesh_inspector_content() override;

private:
    Ui::mesh_inspector_content *ui;
};
} // namespace editor

#endif // MESH_INSPECTOR_CONTENT_H
