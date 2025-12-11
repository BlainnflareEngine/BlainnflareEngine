//
// Created by gorev on 18.11.2025.
//

#pragma once
#include <qspinbox.h>

class NumericInputWidget : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit NumericInputWidget(QWidget *parent = nullptr)
        : QDoubleSpinBox(parent)
    {
        setStyleSheet(R"(
            border-radius: 5px;
    )");

        auto f = font();
        f.setBold(false);
        f.setWeight(QFont::Normal);
        setFont(f);

        connect(this, &NumericInputWidget::valueChanged, this, [this]() { emit ValueChanged(); });
    }


    void focusOutEvent(QFocusEvent *event) override
    {
        emit ValueChanged();
        QDoubleSpinBox::focusOutEvent(event);
    }

signals:
    void ValueChanged();
};
