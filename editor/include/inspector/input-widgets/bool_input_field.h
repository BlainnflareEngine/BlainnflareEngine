//
// Created by gorev on 29.11.2025.
//

#pragma once
#include <QCheckBox>

namespace editor
{

class bool_input_field : public QCheckBox
{
    Q_OBJECT

public:
    explicit bool_input_field(const QString &name, bool defaultValue = false, QWidget *parent = nullptr);
};

} // namespace editor