//
// Created by gorev on 29.11.2025.
//

#include "../../../include/inspector/input-widgets/bool_input_field.h"

namespace editor
{
bool_input_field::bool_input_field(const QString &name, bool defaultValue, QWidget *parent)
    : QCheckBox(name, parent)
{
    setChecked(defaultValue);
}
} // namespace editor