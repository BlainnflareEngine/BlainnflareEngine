#pragma once

#include <qspinbox.h>

namespace editor
{

class float_input_field : public QDoubleSpinBox
{
    Q_OBJECT

public:
    explicit float_input_field(QWidget *parent = nullptr);
    explicit float_input_field(float value, QWidget *parent = nullptr);

    void SetValue(float value);
    float GetValue() const;

protected:
    void focusOutEvent(QFocusEvent *event) override;

public slots:
    void OnEditingFinished();

signals:
    void EditingFinished();

private:
    float m_minValue = -10000.0f;
    float m_maxValue = 10000.0f;
    int m_decimals = 3;
    float m_dragSensitivity = 0.1f;

    bool m_dragging = false;
    QPoint m_lastMousePos;
    double m_dragStartValue = 0.0;

    void Initialize();
};

} // namespace editor