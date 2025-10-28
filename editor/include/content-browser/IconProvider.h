//
// Created by gorev on 08.10.2025.
//

#pragma once
#include <QFileIconProvider>
#include <QIcon>

namespace editor
{

class IconProvider : public QFileIconProvider
{
public:
    QIcon icon(IconType) const override;
    QIcon icon(const QFileInfo &) const override;
};

} // namespace editor