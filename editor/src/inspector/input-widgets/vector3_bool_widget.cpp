//
// Created by gorev on 21.01.2026.
//

#include "input-widgets/vector3_bool_widget.h"

#include "LabelsUtils.h"
#include "input-widgets/bool_input_field.h"

#include <QGridLayout>
#include <QLabel>

editor::vector3_bool_widget::vector3_bool_widget(const QString &label, const BoolVector3 &defaultValue, QWidget *parent)
{
    auto *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0, 5, 0, 5);
    gridLayout->setVerticalSpacing(10);
    gridLayout->setHorizontalSpacing(10);

    auto *titleLabel = new QLabel(ToHeader3(label), this);
    titleLabel->setTextFormat(Qt::MarkdownText);
    titleLabel->setStyleSheet("font-weight: bold;");
    gridLayout->addWidget(titleLabel, 0, 0, 1, 3, Qt::AlignLeft);

    QColor xColor = QColor::fromRgb(255, 68, 68);
    QColor yColor = QColor::fromRgb(68, 255, 68);
    QColor zColor = QColor::fromRgb(68, 68, 255);

    QSize fieldSize{70, 20};

    m_x = new bool_input_field("X", defaultValue.x, this, xColor);
    m_x->setMinimumSize(fieldSize);
    m_x->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(m_x, 1, 0);

    m_y = new bool_input_field("Y", defaultValue.y, this, yColor);
    m_y->setMinimumSize(fieldSize);
    m_y->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(m_y, 1, 1);

    m_z = new bool_input_field("Z", defaultValue.z, this, zColor);
    m_z->setMinimumSize(fieldSize);
    m_z->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(m_z, 1, 2);

    connect(m_x, &bool_input_field::toggled, [this]() { Toggled({GetValue()}); });
    connect(m_y, &bool_input_field::toggled, [this]() { Toggled({GetValue()}); });
    connect(m_z, &bool_input_field::toggled, [this]() { Toggled({GetValue()}); });
}


editor::BoolVector3 editor::vector3_bool_widget::GetValue() const
{
    return {m_x->isChecked(), m_y->isChecked(), m_z->isChecked()};
}


bool editor::vector3_bool_widget::GetX() const
{
    return m_x->isChecked();
}


bool editor::vector3_bool_widget::GetY() const
{
    return m_y->isChecked();
}


bool editor::vector3_bool_widget::GetZ() const
{
    return m_z->isChecked();
}


void editor::vector3_bool_widget::SetValue(const BoolVector3 &value)
{
    m_x->setChecked(value.x);
}


void editor::vector3_bool_widget::SetX(bool x)
{
    m_x->setChecked(x);
}


void editor::vector3_bool_widget::SetY(bool y)
{
    m_y->setChecked(y);
}


void editor::vector3_bool_widget::SetZ(bool z)
{
    m_z->setChecked(z);
}