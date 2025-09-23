//
// Created by gorev on 23.09.2025.
//

#ifndef FOLDERS_WIDGET_H
#define FOLDERS_WIDGET_H

#include <EASTL/unique_ptr.h>
#include <QWidget>
#include <qdir.h>


class QTreeView;
class QFileSystemModel;
class QVBoxLayout;

namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class folders_widget;
}
QT_END_NAMESPACE

class folders_widget : public QWidget
{
    Q_OBJECT

public:
    explicit folders_widget(QWidget *parent = nullptr);
    ~folders_widget() override;

    void SetContentDirectory(const QString &contentDirectory);

signals:
    void folderSelected(const QString &path);

private slots:
    void onSelectionChanged(const QModelIndex &newSelection);

private:
    eastl::unique_ptr<Ui::folders_widget> m_ui;
    eastl::unique_ptr<QTreeView> m_treeView;
    eastl::unique_ptr<QFileSystemModel> m_fileSystemModel;
    eastl::unique_ptr<QVBoxLayout> m_layout;
};
} // namespace editor

#endif // FOLDERS_WIDGET_H
