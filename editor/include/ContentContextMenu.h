//
// Created by gorev on 23.09.2025.
//

#pragma once
#include <QFileSystemModel>


class QAbstractItemView;
namespace editor
{

class ContentContextMenu : public QObject
{
    Q_OBJECT
public:
    ContentContextMenu(QAbstractItemView *parent);

public slots:
    void onContextMenu(const QPoint &pos);

private:
    QAbstractItemView *parent;
};

} // namespace editor
