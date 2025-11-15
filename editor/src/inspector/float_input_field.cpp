#include "float_input_field.h"

#include "oclero/qlementine/widgets/LineEdit.hpp"

#include <QDoubleValidator>
#include <QMouseEvent>

namespace editor
{
float_input_field::float_input_field(QWidget *parent)
    : QDoubleSpinBox(parent)
{
    Initialize();
    setValue(0);
}

float_input_field::float_input_field(float value, QWidget *parent)
    : QDoubleSpinBox(parent)
{
    Initialize();
    setValue(value);
}


void float_input_field::SetValue(float value)
{
    setValue(qBound(m_minValue, value, m_maxValue));
}


float float_input_field::GetValue() const
{
    return static_cast<float>(value());
}


void float_input_field::focusOutEvent(QFocusEvent *event)
{
    emit EditingFinished();
    QDoubleSpinBox::focusOutEvent(event);
}


void float_input_field::OnEditingFinished()
{
    emit EditingFinished();
}


void float_input_field::Initialize()
{
    setButtonSymbols(QAbstractSpinBox::NoButtons);

    setRange(m_minValue, m_maxValue);
    setDecimals(m_decimals);
    setSingleStep(0.01);


    setStyleSheet(R"(
        QDoubleSpinBox {
            border-radius: 5px;
        }
    )");

    if (QLineEdit *editor = lineEdit())
    {
        editor->installEventFilter(this);
        editor->setMouseTracking(true);
    }

    connect(this, &float_input_field::editingFinished, this, &float_input_field::OnEditingFinished);
}


} // namespace editor