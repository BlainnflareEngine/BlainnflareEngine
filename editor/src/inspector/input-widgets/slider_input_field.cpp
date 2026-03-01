//
// Created by gorev on 22.01.2026.
//

#include "input-widgets/slider_input_field.h"

#include <QHBoxLayout>
#include <QLabel>
#include <qslider.h>


namespace editor
{
slider_input_field::slider_input_field(const QString &name, float value, float min, float max, QWidget *parent,
                                       bool immediate, QColor nameColor)
    : QWidget(parent)
    , m_minValue(min)
    , m_maxValue(max)
    , m_immediateMode(immediate)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(10);
    layout()->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_label = new QLabel(name, this);
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_label->setCursor(Qt::SizeHorCursor);

    if (nameColor != QColor())
    {
        m_label->setStyleSheet(QString("QLabel { color: %1; }").arg(nameColor.name(QColor::HexRgb)));
    }
    layout()->addWidget(m_label);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(ValueToSlider(m_minValue), ValueToSlider(m_maxValue));
    m_slider->setValue(ValueToSlider(value));
    m_slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout()->addWidget(m_slider);

    if (m_showValue)
    {
        m_valueLabel = new QLabel(this);
        m_valueLabel->setMinimumWidth(40);
        m_valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_valueLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        layout()->addWidget(m_valueLabel);
        m_valueLabel->setText(QString::number(value, 'f', m_decimals));
    }

    if (immediate)
    {
        connect(m_slider, &QSlider::valueChanged, this, &slider_input_field::OnValueChanged);
    }
    else
    {
        connect(m_slider, &QSlider::sliderReleased, this, [this]() { OnValueChanged(m_slider->value()); });
    }

    m_lastValue = value;
}


void slider_input_field::SetValue(float value)
{
    float clampedValue = qBound(m_minValue, value, m_maxValue);
    if (clampedValue == m_lastValue) return;

    m_lastValue = clampedValue;
    m_slider->setValue(ValueToSlider(clampedValue));

    if (m_showValue)
    {
        m_valueLabel->setText(QString::number(clampedValue, 'f', m_decimals));
    }
}


float slider_input_field::GetValue() const
{
    return m_lastValue;
}


void slider_input_field::SetRange(float min, float max)
{
    m_minValue = min;
    m_maxValue = max;
    m_slider->setRange(ValueToSlider(m_minValue), ValueToSlider(m_maxValue));
    SetValue(m_lastValue);
}


void slider_input_field::SetMinValue(float min)
{
    SetRange(min, m_maxValue);
}


void slider_input_field::SetMaxValue(float max)
{
    SetRange(m_minValue, max);
}


void slider_input_field::SetSingleStep(float step)
{
    m_singleStep = step;
}


bool slider_input_field::HasFocus() const
{
    return m_slider->hasFocus() || m_label->hasFocus();
}


void slider_input_field::OnValueChanged(int value)
{
    float newValue = SliderToValue(value);
    if (newValue != m_lastValue)
    {
        m_lastValue = newValue;
        if (m_showValue)
        {
            m_valueLabel->setText(QString::number(newValue, 'f', m_decimals));
        }
        emit ValueChanged();
    }
}


int slider_input_field::ValueToSlider(float value) const
{
    if (m_maxValue <= m_minValue) return 0;

    const int SLIDER_RANGE = 10000;
    float normalized = (value - m_minValue) / (m_maxValue - m_minValue);
    return static_cast<int>(normalized * SLIDER_RANGE);
}


float slider_input_field::SliderToValue(int sliderValue) const
{
    if (m_maxValue <= m_minValue) return m_minValue;

    const int SLIDER_RANGE = 10000;
    float normalized = static_cast<float>(sliderValue) / SLIDER_RANGE;
    return m_minValue + normalized * (m_maxValue - m_minValue);
}
} // namespace editor