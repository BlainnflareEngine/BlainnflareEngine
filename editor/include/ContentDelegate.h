//
// Created by gorev on 17.10.2025.
//

#pragma once

#include <QStyledItemDelegate>


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

    explicit ContentDelegate(TextMode mode = Wrap, QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    void DrawStandard(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void DrawTexturePreview(QFileInfo &filePath, QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const;
    void DrawBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
                        bool drawIcon = true) const;
    void DrawName(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const QRect &iconRect) const;

    QRect CalculateIconRect(const QRect &itemRect) const;
    QRect CalculateTextRect(const QRect &itemRect, const QRect &iconRect) const;
    QPixmap GetTexturePixmap(const QFileInfo &filePath) const;

    TextMode m_textMode;
    unsigned int m_iconSize = 48;
    unsigned int m_spacing = 5;
};

} // namespace editor