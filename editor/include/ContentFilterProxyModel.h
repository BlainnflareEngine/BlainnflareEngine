//
// Created by gorev on 22.10.2025.
//

#pragma once
#include <QSortFilterProxyModel>

namespace editor
{

class ContentFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ContentFilterProxyModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

} // namespace editor