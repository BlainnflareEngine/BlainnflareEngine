//
// Created by gorev on 14.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_inspector_widget.h" resolved

#include "inspector_widget.h"
#include "Profiler.h"
#include "ui_inspector_widget.h"

#include <qlayout.h>

namespace editor
{
inspector_widget::inspector_widget(QWidget *parent)
    : QScrollArea(parent)
    , ui(new Ui::inspector_widget)
{
    ui->setupUi(this);

    setWidget(new QWidget());

    setAutoFillBackground(false);
    setWidgetResizable(true);
}


inspector_widget::~inspector_widget()
{
    delete ui;
}


void inspector_widget::SetItem(QWidget *item)
{
    BLAINN_PROFILE_FUNC();

    setUpdatesEnabled(false);

    if (m_locked) return;

    {
        BLAINN_PROFILE_SCOPE(DeleteLater);
        if (widget()) widget()->deleteLater();
    }

    if (!item) return;

    {
        BLAINN_PROFILE_SCOPE(SetWidget);
        setWidget(item);
    }

    setUpdatesEnabled(true);
}
QSize inspector_widget::sizeHint() const
{
    BLAINN_PROFILE_FUNC();

    return QScrollArea::sizeHint();
}

QSize inspector_widget::minimumSizeHint() const
{
    BLAINN_PROFILE_FUNC();

    return QScrollArea::minimumSizeHint();
}
void inspector_widget::resizeEvent(QResizeEvent *event)
{
    BLAINN_PROFILE_FUNC();

    QScrollArea::resizeEvent(event);
}

void inspector_widget::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();
    QScrollArea::paintEvent(event);
}


bool inspector_widget::IsLocked() const
{
    return m_locked;
}


void inspector_widget::SetLocked(const bool locked)
{
    m_locked = locked;
}

} // namespace editor
