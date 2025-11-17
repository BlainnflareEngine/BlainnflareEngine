//
// Created by gorev on 15.11.2025.
//

#include "themed_panel.h"
#include <oclero/qlementine/style/QlementineStyle.hpp>

#include <QPainter>
#include <QPainterPath>

namespace editor
{


themed_panel::themed_panel(QWidget *parent)
    : QFrame(parent)
{
}


void themed_panel::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor backgroundColor;
    int radius;

    if (auto *style = oclero::qlementine::appStyle())
    {
        backgroundColor = style->theme().backgroundColorMain4;
        radius = style->theme().borderRadius;
    }
    else
    {
        backgroundColor = palette().color(QPalette::Window);
        radius = 5;
    }


    QPainterPath path;
    path.addRoundedRect(rect(), radius, radius);
    p.fillPath(path, backgroundColor);
}
} // namespace editor