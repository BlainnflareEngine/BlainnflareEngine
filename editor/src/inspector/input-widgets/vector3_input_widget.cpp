//
// Created by gorev on 07.12.2025.
//

#include "../../../include/inspector/input-widgets/vector3_input_widget.h"

#include "LabelsUtils.h"
#include "../../../include/inspector/input-widgets/float_input_field.h"
#include "oclero/qlementine/widgets/Label.hpp"

#include <QGridLayout>

namespace editor
{
vector3_input_widget::vector3_input_widget(const QString &title, const Blainn::Vec3 &value, bool immediate, QWidget *parent)
{
    auto *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0, 5, 0, 5);
    gridLayout->setVerticalSpacing(10);
    gridLayout->setHorizontalSpacing(10);

    auto *titleLabel = new QLabel(ToHeader3(title), this);
    titleLabel->setTextFormat(Qt::MarkdownText);
    titleLabel->setStyleSheet("font-weight: bold;");
    gridLayout->addWidget(titleLabel, 0, 0, 1, 3, Qt::AlignLeft);

    QColor xColor = QColor::fromRgb(255, 68, 68);
    QColor yColor = QColor::fromRgb(68, 255, 68);
    QColor zColor = QColor::fromRgb(68, 68, 255);

    QSize fieldSize{70, 20};

    m_x = new float_input_field("X", value.x, this, immediate, xColor);
    m_x->setMinimumSize(fieldSize);
    m_x->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(m_x, 1, 0);

    m_y = new float_input_field("Y", value.y, this, immediate, yColor);
    m_y->setMinimumSize(fieldSize);
    m_y->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(m_y, 1, 1);

    m_z = new float_input_field("Z", value.z, this, immediate, zColor);
    m_z->setMinimumSize(fieldSize);
    m_z->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(m_z, 1, 2);

    connect(m_x, &float_input_field::EditingFinished, this, &vector3_input_widget::OnComponentEditingFinished);
    connect(m_y, &float_input_field::EditingFinished, this, &vector3_input_widget::OnComponentEditingFinished);
    connect(m_z, &float_input_field::EditingFinished, this, &vector3_input_widget::OnComponentEditingFinished);
}


Blainn::Vec3 vector3_input_widget::GetValue() const
{
    return {m_x->GetValue(), m_y->GetValue(), m_z->GetValue()};
}


void vector3_input_widget::SetValue(const Blainn::Vec3 &value)
{
    m_x->SetValue(value.x);
    m_y->SetValue(value.y);
    m_z->SetValue(value.z);
}


void vector3_input_widget::BlockSignals(bool block)
{
    m_x->blockSignals(block);
    m_y->blockSignals(block);
    m_z->blockSignals(block);
}


bool vector3_input_widget::HasFocus() const
{
    return m_x->HasFocus() || m_y->HasFocus() || m_z->HasFocus();
}


int vector3_input_widget::GetDecimals() const
{
    return m_x->GetDecimals();
}


void vector3_input_widget::SetDecimals(int value)
{
    m_x->SetDecimals(value);
    m_y->SetDecimals(value);
    m_z->SetDecimals(value);
}


void vector3_input_widget::SetSingleStep(float step) const
{
    m_x->SetSingleStep(step);
    m_y->SetSingleStep(step);
    m_z->SetSingleStep(step);
}

void vector3_input_widget::SetMinValue(float value)
{
    m_x->SetMinValue(value);
    m_y->SetMinValue(value);
    m_z->SetMinValue(value);
}

void vector3_input_widget::SetMaxValue(float value)
{
    m_x->SetMaxValue(value);
    m_y->SetMaxValue(value);
    m_z->SetMaxValue(value);
}


void vector3_input_widget::OnComponentEditingFinished()
{
    Blainn::Vec3 value = GetValue();
    emit ValueChanged(value);
    emit EditingFinished();
}
} // namespace editor