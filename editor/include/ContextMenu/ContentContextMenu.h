//
// Created by gorev on 23.09.2025.
//

#pragma once
#include "EASTL/compare.h"


#include <QFileSystemModel>


class QAbstractItemView;
namespace editor
{

class ContentContextMenu : public QObject
{
    Q_OBJECT
public:
    ContentContextMenu(QAbstractItemView &parent);

public slots:
    void OnContextMenu(const QPoint &pos) const;

private:
    void CreateFolder(QString dirPath) const;
    void CreateScript(const QString &dirPath) const;

    QAbstractItemView &m_parent;
};

} // namespace editor
