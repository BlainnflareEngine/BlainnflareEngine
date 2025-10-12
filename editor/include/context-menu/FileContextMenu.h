//
// Created by gorev on 24.09.2025.
//

#pragma once
#include "EASTL/vector.h"


#include <qabstractitemview.h>
#include <qtmetamacros.h>

namespace editor
{

class FileContextMenu : public QObject
{
    Q_OBJECT
public:
    explicit FileContextMenu(QAbstractItemView &parent, const eastl::vector<QAbstractItemView *> &additionalViews = {});

    void FileContext(const QPoint &pos, const QString &path) const;

    void DirectoryContext(const QPoint &pos, const QString &path) const;

    void OnContextMenu(const QPoint &pos) const;

    void AddAdditionalView(QAbstractItemView* view);

    void RemoveAdditionalView(QAbstractItemView* view);

private:
    QAbstractItemView &m_parent;
    eastl::vector<QAbstractItemView *> m_additionalViews;
};

} // namespace editor
