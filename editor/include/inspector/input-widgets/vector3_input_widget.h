//
// Created by gorev on 07.12.2025.
//

#pragma once

#include "aliases.h"


#include <QWidget>

namespace editor
{
class float_input_field;

class vector3_input_widget : public QWidget
{
    Q_OBJECT;

public:
    explicit vector3_input_widget(const QString &title, const Blainn::Vec3 &value = Blainn::Vec3::Zero, bool immediate = true,
                                  QWidget *parent = nullptr);

    Blainn::Vec3 GetValue() const;
    void SetValue(const Blainn::Vec3 &value);

    void BlockSignals(bool block);

    bool HasFocus() const;

    int GetDecimals() const;
    void SetDecimals(int value);

    void SetSingleStep(float step) const;
    void SetMinValue(float value);
    void SetMaxValue(float value);

signals:
    void ValueChanged(const Blainn::Vec3 &value);
    void EditingFinished();

private slots:
    void OnComponentEditingFinished();

private:
    float_input_field *m_x = nullptr;
    float_input_field *m_y = nullptr;
    float_input_field *m_z = nullptr;
};

} // namespace editor