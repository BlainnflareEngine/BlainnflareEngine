//
// Created by gorev on 23.09.2025.
//

#pragma once

#include <QWidget>


class QAbstractItemView;
namespace editor
{
class folder_content_list_view;
class ContentContextMenu;
class FileContextMenu;
class ContentFilterProxyModel;
class IconProvider;
}
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

    void AddAdditionalView(QAbstractItemView *view);
    void RemoveAdditionalView(QAbstractItemView *view);

    QListView *GetListView() const;

public slots:
    void OnFolderSelectedPath(const QString &newPath);
    void OnEntrySelectedIndex(const QModelIndex &newSelection);
    void OnFileSelectedPath(const QModelIndex &index);

signals:
    void FolderSelected(const QString &path);
    void FileSelected(const QString &filePath);

private:
    Ui::folder_content_widget *ui;
    folder_content_list_view *m_listView;
    ContentFilterProxyModel *m_proxyModel;
    QFileSystemModel *m_fileSystemModel;
    QVBoxLayout *m_layout;
    ContentContextMenu *m_contentContextMenu;
    FileContextMenu *m_fileContextMenu;
    IconProvider *m_iconProvider;
};
} // namespace editor

