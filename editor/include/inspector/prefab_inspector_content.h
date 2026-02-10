//
// Created by gorev on 09.02.2026.
//

#pragma once
#include "inspector_content_base.h"


class QPushButton;
namespace editor
{

class prefab_inspector_content : public inspector_content_base
{
    Q_OBJECT
public:
    explicit prefab_inspector_content(const QString &file, QObject *parent = nullptr);

private:
    QString m_file;

    QPushButton *m_applyChanges = nullptr;
    QPushButton *m_revertChanges = nullptr;
};

} // namespace editor