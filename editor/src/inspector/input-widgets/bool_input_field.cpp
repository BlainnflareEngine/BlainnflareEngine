//
// Created by gorev on 29.11.2025.
//

#include "../../../include/inspector/input-widgets/bool_input_field.h"

namespace editor
{
bool_input_field::bool_input_field(const QString &name, bool defaultValue, const QColor &color, QWidget *parent)
    : QCheckBox(name, parent)
{
    setChecked(defaultValue);

    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, color);
    palette.setColor(QPalette::ButtonText, color);
    setPalette(palette);

    setStyleSheet(QString("QCheckBox { color: %1; }"
                          "QCheckBox::indicator { width: 16px; height: 16px; }")
                      .arg(color.name()));
}
} // namespace editor