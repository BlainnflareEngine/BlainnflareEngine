//
// Created by gorev on 17.10.2025.
//

#pragma once


#include <QCache>
#include <QStyledItemDelegate>
#include <QThreadPool>


namespace oclero::qlementine
{
class LoadingSpinner;
}
class QSortFilterProxyModel;
class QFileSystemModel;
class QFileInfo;
namespace editor
{

class ContentDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum TextMode
    {
        Wrap,
        Elide
    };

    explicit ContentDelegate(TextMode mode, QFileSystemModel *model, QSortFilterProxyModel *proxy = nullptr,
                             QObject *parent = nullptr);

    ~ContentDelegate() override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

public slots:
    void OnTextureLoaded(const QString &filePath, const QPixmap &pixmap, const QPersistentModelIndex &index);

protected:
    void DrawStandard(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void DrawTexturePreview(QFileInfo &filePath, QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const;
    void DrawBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                        bool drawIcon = true) const;
    void DrawName(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                  const QRect &iconRect) const;

    QRect CalculateIconRect(const QRect &itemRect) const;
    QRect CalculateTextRect(const QRect &itemRect, const QRect &iconRect) const;

    void StartAsyncLoading(const QModelIndex &index, const QString &path) const;
    QPixmap GetTexturePixmap(const QFileInfo &filePath, const QModelIndex &index) const;

    TextMode m_textMode;
    unsigned int m_iconSize = 48;
    unsigned int m_spacing = 5;

    QFileSystemModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxy = nullptr;
    QThreadPool *m_threadPool = nullptr;
    QPalette m_palette;

    mutable QCache<QString, QPixmap> m_pixmapCache;
    mutable QSet<QString> m_loadingTextures;
};

} // namespace editor