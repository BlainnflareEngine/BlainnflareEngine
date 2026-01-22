//
// Created by gorev on 11.12.2025.
//

#include "input-widgets/color_input_field.h"

#include <QLabel>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QPushButton>

namespace editor
{
color_input_field::color_input_field(const QString &name, QColor value, QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setLayout(new QHBoxLayout());
    layout()->setSpacing(10);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_label = new QLabel(name, this);
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_label->setStyleSheet("QLabel {"
                           "    font-weight: bold;"
                           "}");
    layout()->addWidget(m_label);

    m_button = new QPushButton(this);
    m_button->setFixedSize(20, 20);
    m_button->setText("");
    m_button->setStyleSheet("QPushButton { border: 1px solid #555; }");
    layout()->addWidget(m_button);

    SetValue(value);
    connect(m_button, &QPushButton::clicked, this, &color_input_field::OnColorButtonClicked);
}


void color_input_field::SetValue(const QColor &value)
{
    if (value == m_color) return;

    m_color = value;

    QString style = QString("QPushButton {"
                            "    background-color: %1;"
                            "    border: 1px solid #555;"
                            "}")
                        .arg(m_color.name(QColor::HexRgb));
    m_button->setStyleSheet(style);
}


QColor &color_input_field::GetValue()
{
    return m_color;
}


bool color_input_field::HasFocus() const
{
    return m_button->hasFocus();
}


void color_input_field::OnColorButtonClicked()
{
    if (!m_dialog)
    {
        m_dialog = new QColorDialog(this);
        m_dialog->setOption(QColorDialog::DontUseNativeDialog, true);
        m_dialog->setModal(false);

        connect(m_dialog, &QColorDialog::currentColorChanged, this,
                [this](const QColor &color)
                {
                    if (color.isValid())
                    {
                        SetValue(color);
                        emit EditingFinished();
                    }
                });

        connect(m_dialog, &QColorDialog::finished, this,
                [this](int result)
                {
                    if (result == QDialog::Accepted)
                    {
                        QColor newColor = m_dialog->currentColor();
                        if (newColor.isValid() && newColor != m_color)
                        {
                            SetValue(newColor);
                            emit EditingFinished();
                        }
                    }
                    m_dialog->deleteLater();
                    m_dialog = nullptr;
                });
    }

    m_dialog->setCurrentColor(m_color);
    m_dialog->show();
    m_dialog->raise();
    m_dialog->activateWindow();
}
} // namespace editor