//
// Created by gorev on 14.10.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_inspector_widget.h" resolved

#include "inspector_widget.h"
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
}


inspector_widget::~inspector_widget()
{
    delete ui;
}


void inspector_widget::SetItem(QWidget *item)
{
    if (m_locked) return;

    if (widget())
        widget()->deleteLater();

    if (!item) return;

    setWidget(item);
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
