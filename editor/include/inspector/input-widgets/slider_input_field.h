//
// Created by gorev on 22.01.2026.
//

#pragma once

#include <QWidget>

class QLabel;
class QSlider;
namespace editor
{

class slider_input_field : public QWidget
{
    Q_OBJECT

public:
    struct Range
    {
        float min = 0.0f;
        float max = 100.0f;
    };

    explicit slider_input_field(const QString &name, float value = 0.0f, QWidget *parent = nullptr,
                                bool immediate = true, QColor nameColor = QColor());

    void SetValue(float value);
    float GetValue() const;

    void SetRange(const Range &range);
    void SetMinValue(float min);
    void SetMaxValue(float max);

    void SetSingleStep(float step);

    bool HasFocus() const;

    int GetDecimals() const
    {
        return m_decimals;
    }
    void SetDecimals(int decimals)
    {
        m_decimals = decimals;
    }

public slots:
    void OnValueChanged(int value);

signals:
    void ValueChanged();

private:
    QSlider *m_slider = nullptr;
    QLabel *m_label = nullptr;
    QLabel *m_valueLabel = nullptr;

    float m_lastValue = 0.0f;
    float m_minValue = 0.0f;
    float m_maxValue = 100.0f;
    float m_singleStep = 1.0f;

    bool m_immediateMode = true;
    bool m_showValue = true;

    int m_decimals = 1;

    int ValueToSlider(float value) const;
    float SliderToValue(int sliderValue) const;
};

} // namespace editor
