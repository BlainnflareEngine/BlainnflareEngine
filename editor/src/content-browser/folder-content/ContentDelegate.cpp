//
// Created by gorev on 17.10.2025.
//

#include "ContentDelegate.h"

#include "Editor.h"
#include "FileSystemUtils.h"
#include "TexturePreviewAsyncLoader.h"
#include "oclero/qlementine/widgets/LoadingSpinner.hpp"

#include <QApplication>
#include <QCache>
#include <QFileSystemModel>
#include <QPainter>
#include <qfileinfo.h>
#include <QThreadPool>

class QFileSystemModel;
namespace editor
{

ContentDelegate::ContentDelegate(TextMode mode, QFileSystemModel *model, QSortFilterProxyModel *proxy, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_textMode(mode)
    , m_pixmapCache(100)
    , m_model(model)
    , m_proxy(proxy)
    , m_palette(qApp->palette())
{
    m_threadPool = new QThreadPool(this);
    m_threadPool->setMaxThreadCount(4);
}


ContentDelegate::~ContentDelegate()
{
    m_threadPool->waitForDone();
}


void ContentDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    QModelIndex sourceIndex;
    if (m_proxy)
    {
        sourceIndex = m_proxy->mapToSource(index);
    }

    auto file = QFileInfo(m_model->filePath(sourceIndex));

    if (supportedTextureFormats.contains(file.suffix())) DrawTexturePreview(file, painter, option, index);
    else DrawStandard(painter, option, index);

    painter->restore();
}


QSize ContentDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    QFontMetrics metrics(option.font);

    if (m_textMode == Wrap)
    {
        int textWidth = 100;
        QRect textRect(0, 0, textWidth, 0);
        textRect = metrics.boundingRect(textRect, Qt::TextWordWrap, text);
        int textHeight = textRect.height();
        return QSize(100, m_iconSize + textHeight + m_spacing * 3);
    }
    else
    {
        int textHeight = metrics.height() * 2;
        return QSize(100, m_iconSize + textHeight + m_spacing * 3);
    }
}


void ContentDelegate::OnTextureLoaded(const QString &filePath, const QPixmap &pixmap,
                                      const QPersistentModelIndex &index)
{
    m_pixmapCache.insert(filePath, new QPixmap(pixmap));
    m_loadingTextures.remove(filePath);

    if (index.isValid() && m_proxy)
    {
        Q_EMIT m_proxy->dataChanged(index, index);
    }
}


void ContentDelegate::DrawStandard(QPainter *painter, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    DrawBackground(painter, option, index);

    QVariant iconVar = index.data(Qt::DecorationRole);
    QRect iconRect = CalculateIconRect(option.rect);

    if (!iconVar.isNull())
    {
        QIcon icon = iconVar.value<QIcon>();
        icon.paint(painter, iconRect, Qt::AlignCenter,
                   option.state & QStyle::State_Selected ? QIcon::Selected : QIcon::Normal);
    }

    DrawName(painter, option, index, iconRect);
}


void ContentDelegate::DrawTexturePreview(QFileInfo &filePath, QPainter *painter, const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QString path = filePath.filePath();

    DrawBackground(painter, opt, index, false);

    QPixmap pixmap = GetTexturePixmap(filePath, index);
    QRect iconRect = CalculateIconRect(opt.rect);

    if (!pixmap.isNull()) painter->drawPixmap(iconRect, pixmap);

    QColor borderColor = m_palette.color(QPalette::Light);
    painter->setPen(QPen(borderColor, 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(iconRect);

    DrawName(painter, opt, index, iconRect);
}


void ContentDelegate::DrawBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                                     bool drawIcon) const
{
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();

    if (drawIcon)
    {
        style->drawControl(QStyle::CE_ItemViewItem, &option, painter, option.widget);
    }
    else
    {
        QStyleOptionViewItem backgroundOpt = option;
        backgroundOpt.icon = QIcon();
        backgroundOpt.text = "";
        style->drawControl(QStyle::CE_ItemViewItem, &backgroundOpt, painter, option.widget);
    }
}


void ContentDelegate::DrawName(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                               const QRect &iconRect) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    QRect textRect = CalculateTextRect(option.rect, iconRect);

    if (option.state & QStyle::State_Selected)
    {
        painter->setPen(option.palette.highlightedText().color());
    }
    else
    {
        painter->setPen(option.palette.text().color());
    }

    if (m_textMode == Wrap)
    {
        painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, text);
    }
    else if (m_textMode == Elide)
    {
        QFontMetrics metrics(option.font);
        QString elidedText = metrics.elidedText(text, Qt::ElideRight, textRect.width());
        painter->drawText(textRect, Qt::AlignCenter, elidedText);
    }
}


QRect ContentDelegate::CalculateIconRect(const QRect &itemRect) const
{
    return QRect(itemRect.left() + (itemRect.width() - m_iconSize) / 2, itemRect.top() + m_spacing, m_iconSize,
                 m_iconSize);
}


QRect ContentDelegate::CalculateTextRect(const QRect &itemRect, const QRect &iconRect) const
{
    return QRect(itemRect.left(), itemRect.top() + iconRect.height() + m_spacing * 2, itemRect.width(),
                 itemRect.height() - iconRect.height() - m_spacing * 3);
}


void ContentDelegate::StartAsyncLoading(const QModelIndex &index, const QString &path) const
{
    m_loadingTextures.insert(path);

    auto spinner = new oclero::qlementine::LoadingSpinner();
    spinner->setSpinning(true);

    auto *loader = new TexturePreviewAsyncLoader(path, QPersistentModelIndex(index));

    connect(loader, &TexturePreviewAsyncLoader::loaded, this, &ContentDelegate::OnTextureLoaded, Qt::QueuedConnection);

    m_threadPool->start(loader);
}


QPixmap ContentDelegate::GetTexturePixmap(const QFileInfo &filePath, const QModelIndex &index) const
{
    QString path = filePath.filePath();

    if (QPixmap *cachedPixmap = m_pixmapCache.object(path))
    {
        return *cachedPixmap;
    }

    if (m_loadingTextures.contains(path))
    {
        return QPixmap();
    }

    StartAsyncLoading(index, path);

    return QPixmap();
}
} // namespace editor