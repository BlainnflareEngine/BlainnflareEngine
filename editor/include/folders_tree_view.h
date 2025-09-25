//
// Created by gorev on 25.09.2025.
//

#pragma once

#include "EASTL/compare.h"
#include "EASTL/unique_ptr.h"


#include <QTreeView>

namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class folders_tree_view;
}
QT_END_NAMESPACE

class folders_tree_view : public QTreeView
{
    Q_OBJECT

public:
    explicit folders_tree_view(QWidget *parent = nullptr);
    ~folders_tree_view() override;

protected:
    void dropEvent(QDropEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;

    void dragMoveEvent(QDragMoveEvent *event) override;

private:
    eastl::unique_ptr<Ui::folders_tree_view> ui;
};
} // namespace editor
