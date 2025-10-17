//
// Created by gorev on 17.10.2025.
//

#include "ContentDelegate.h"

#include <QApplication>
#include <QPainter>

namespace editor
{
ContentDelegate::ContentDelegate(TextMode mode, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_textMode(mode)
{
}


void ContentDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyle *style = option.widget ? option.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &option, painter, option.widget);

    QVariant iconVar = index.data(Qt::DecorationRole);
    QString text = index.data(Qt::DisplayRole).toString();

    QRect rect = option.rect;
    int iconSize = 48;
    int spacing = 5;

    QRect iconRect(rect.left() + (rect.width() - iconSize) / 2, rect.top() + spacing, iconSize, iconSize);
    if (!iconVar.isNull())
    {
        QIcon icon = iconVar.value<QIcon>();
        icon.paint(painter, iconRect, Qt::AlignCenter,
                   option.state & QStyle::State_Selected ? QIcon::Selected : QIcon::Normal);
    }

    QRect textRect(rect.left(), rect.top() + iconSize + spacing * 2, rect.width(),
                   rect.height() - iconSize - spacing * 3);

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


QSize ContentDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int iconSize = 48;
    int spacing = 5;

    // Получите текст для расчета высоты
    QString text = index.data(Qt::DisplayRole).toString();
    QFontMetrics metrics(option.font);

    if (m_textMode == Wrap)
    {
        int textWidth = 100;
        QRect textRect(0, 0, textWidth, 0);
        textRect = metrics.boundingRect(textRect, Qt::TextWordWrap, text);
        int textHeight = textRect.height();
        return QSize(100, iconSize + textHeight + spacing * 3);
    }
    else
    {
        int textHeight = metrics.height() * 2;
        return QSize(100, iconSize + textHeight + spacing * 3);
    }
}
} // namespace editor