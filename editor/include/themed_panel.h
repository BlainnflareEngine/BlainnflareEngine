//
// Created by gorev on 15.11.2025.
//

#pragma once
#include <QFrame>

namespace editor
{

class themed_panel : public QFrame
{
    Q_OBJECT

protected:
    explicit themed_panel(QWidget *parent);

    void paintEvent(QPaintEvent *event) override;
};

} // namespace editor