//
// Created by gorev on 17.10.2025.
//

#include "ContentDelegate.h"

#include "Editor.h"
#include "FileSystemUtils.h"

#include <QApplication>
#include <QCache>
#include <QPainter>
#include <qfileinfo.h>

namespace editor
{
ContentDelegate::ContentDelegate(TextMode mode, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_textMode(mode)
{
}


void ContentDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    auto file = QFileInfo(index.data().toString());

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

    DrawBackground(painter, opt, index, false);

    QPixmap pixmap = GetTexturePixmap(filePath);
    QRect iconRect = CalculateIconRect(opt.rect);

    painter->drawPixmap(iconRect, pixmap);

    painter->setPen(QPen(Qt::darkGray, 1));
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


QPixmap ContentDelegate::GetTexturePixmap(const QFileInfo &filePath) const
{
    QString path = QString::fromStdString(Blainn::Editor::GetInstance().GetContentDirectory().string())
                   + QDir::separator() + filePath.fileName();

    static QCache<QString, QPixmap> pixmapCache(100);
    QPixmap *cachedPixmap = pixmapCache.object(path);

    if (cachedPixmap)
    {
        return *cachedPixmap;
    }

    QPixmap pixmap(path);
    if (!pixmap.isNull())
    {
        pixmap = pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pixmapCache.insert(path, new QPixmap(pixmap));
    }
    else
    {
        pixmap = QPixmap(64, 64);
        pixmap.fill(Qt::lightGray);
        QPainter placeholderPainter(&pixmap);
        placeholderPainter.setPen(Qt::black);
        placeholderPainter.drawText(pixmap.rect(), Qt::AlignCenter, "Error");
    }

    return pixmap;
}
} // namespace editor