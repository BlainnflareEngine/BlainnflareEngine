#include "input-widgets/float_input_field.h"

#include "input-widgets/NumericInputWidget.h"
#include "oclero/qlementine/style/QlementineStyle.hpp"

#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <qspinbox.h>

namespace editor
{

float_input_field::float_input_field(const QString &name, float value, QWidget *parent, QColor nameColor)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(10);
    layout()->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_label = new QLabel(name, this);
    m_label->adjustSize();
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    if (nameColor != QColor())
    {
        m_label->setStyleSheet(QString("QLabel {"
                                       "    color: %1;"
                                       "    font-weight: bold;"
                                       "}")
                                   .arg(nameColor.name(QColor::HexRgb)));
    }


    layout()->addWidget(m_label);

    m_input = new NumericInputWidget(this);
    layout()->addWidget(m_input);

    m_input->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_input->setLocale(QLocale::c());
    m_input->setRange(m_minValue, m_maxValue);
    m_input->setDecimals(m_decimals);
    m_input->setSingleStep(0.01);
    m_input->setValue(value);


    m_input->setStyleSheet(R"(
        QDoubleSpinBox {
            border-radius: 5px;
        }
    )");


    connect(m_input, &NumericInputWidget::editingFinished, this, &float_input_field::OnEditingFinished);
    connect(m_input, &NumericInputWidget::FocusOut, this, &float_input_field::OnEditingFinished);
}


void float_input_field::SetValue(float value)
{
    m_input->setValue(qBound(m_minValue, value, m_maxValue));
}


float float_input_field::GetValue() const
{
    return static_cast<float>(m_input->value());
}


bool float_input_field::HasFocus() const
{
    return m_input->hasFocus() || m_label->hasFocus();
}


void float_input_field::OnEditingFinished()
{
    emit EditingFinished();
}

} // namespace editor