//
// Created by gorev on 23.09.2025.
//

#ifndef FOLDER_CONTENT_WIDGET_H
#define FOLDER_CONTENT_WIDGET_H

#include "ContextMenu/ContentContextMenu.h"
#include "ContextMenu/FileContextMenu.h"
#include "EASTL/unique_ptr.h"
#include "folder_content_list_view.h"
#include <QWidget>


class QVBoxLayout;
class QFileSystemModel;
class QListView;
namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class folder_content_widget;
}
QT_END_NAMESPACE

class folder_content_widget : public QWidget
{
    Q_OBJECT

public:
    explicit folder_content_widget(QWidget *parent = nullptr);
    ~folder_content_widget() override;

    void SetContentDirectory(const QString &contentDirectory) const;

    void AddAdditionalView(QAbstractItemView* view);
    void RemoveAdditionalView(QAbstractItemView* view);

    QListView* GetListView() const;

public slots:
    void onFolderSelectedPath(const QString &newPath);
    void onEntrySelectedIndex(const QModelIndex &newSelection);

signals:
    void folderSelected(const QString &newPath);

private:
    eastl::unique_ptr<Ui::folder_content_widget> ui;
    eastl::unique_ptr<folder_content_list_view> m_listView;
    eastl::unique_ptr<QFileSystemModel> m_fileSystemModel;
    eastl::unique_ptr<QVBoxLayout> m_layout;
    eastl::unique_ptr<ContentContextMenu> m_contentContextMenu;
    eastl::unique_ptr<FileContextMenu> m_fileContextMenu;
};
} // namespace editor

#endif // FOLDER_CONTENT_WIDGET_H
