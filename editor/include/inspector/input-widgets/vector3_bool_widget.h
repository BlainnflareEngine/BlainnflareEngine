//
// Created by gorev on 21.01.2026.
//

#pragma once
#include <QWidget>

namespace editor
{
class bool_input_field;

struct BoolVector3
{
    bool x = false, y = false, z = false;
};

class vector3_bool_widget : public QWidget
{
    Q_OBJECT

public:
    explicit vector3_bool_widget(const QString &label, const BoolVector3 &defaultValue = {}, QWidget *parent = nullptr);

    BoolVector3 GetValue() const;
    bool GetX() const;
    bool GetY() const;
    bool GetZ() const;

    void SetValue(const BoolVector3 &value);
    void SetX(bool x);
    void SetY(bool y);
    void SetZ(bool z);

    bool AllSet() const;

signals:
    void Toggled(const BoolVector3 &value);

private:
    bool_input_field *m_x = nullptr;
    bool_input_field *m_y = nullptr;
    bool_input_field *m_z = nullptr;
};
} // namespace editor
