//
// Created by gorev on 17.10.2025.
//

#pragma once

#include <QStyledItemDelegate>

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

private:
    TextMode m_textMode;
};

} // namespace editor