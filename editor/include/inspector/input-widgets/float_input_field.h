#pragma once

#include <QWidget>


class NumericInputWidget;
class QLabel;


namespace editor
{

class float_input_field : public QWidget
{
    Q_OBJECT

public:
    explicit float_input_field(const QString &name, float value = 0, QWidget *parent = nullptr,bool immediate = true, 
                               QColor nameColor = QColor());

    void SetValue(float value);
    float GetValue() const;

    void SetMinValue(float min) const;
    void SetMaxValue(float max) const;

    void SetSingleStep(float step) const;

    bool HasFocus() const;

    int GetDecimals() const;
    void SetDecimals(int value);

public slots:
    void OnEditingFinished();

signals:
    void EditingFinished();

private:
    NumericInputWidget *m_input = nullptr;
    QLabel *m_label = nullptr;

    float m_lastValue;

    float m_minValue = -100000.0f;
    float m_maxValue = 100000.0f;
    int m_decimals = 1;

    // TODO: make drag
    float m_dragSensitivity = 0.1f;
    bool m_dragging = false;
    QPoint m_lastMousePos;
    double m_dragStartValue = 0.0;

    bool m_immediateMode = true;
};

} // namespace editor