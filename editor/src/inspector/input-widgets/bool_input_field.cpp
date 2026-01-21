//
// Created by gorev on 29.11.2025.
//

#include "input-widgets/bool_input_field.h"


#include <QHBoxLayout>
#include <QLabel>


namespace editor
{
bool_input_field::bool_input_field(const QString &name, bool defaultValue, QWidget *parent, const QColor &color)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    m_checkbox = new QCheckBox(this);
    m_checkbox->setChecked(defaultValue);

    m_label = new QLabel(name, this);
    m_label->setStyleSheet(QString("QLabel { color: %1; font-weight: normal; }").arg(color.name()));

    layout->addWidget(m_checkbox);
    layout->addWidget(m_label);
    layout->addStretch();

    connect(m_checkbox, &QCheckBox::toggled, this, &bool_input_field::toggled);
}

bool bool_input_field::isChecked() const
{
    return m_checkbox->isChecked();
}

void bool_input_field::setChecked(bool checked)
{
    m_checkbox->setChecked(checked);
}
} // namespace editor