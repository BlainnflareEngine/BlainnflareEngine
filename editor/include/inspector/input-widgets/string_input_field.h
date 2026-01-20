//
// Created by gorev on 19.01.2026.
//

#pragma once


#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>

namespace editor
{
class string_input_field : public QWidget
{
    Q_OBJECT

public:
    explicit string_input_field(const QString &name, const QString &defaultValue = "", const QString &placeHolder = "",
                                QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setLayout(new QHBoxLayout());
        layout()->setContentsMargins(0, 0, 0, 0);
        layout()->setSpacing(10);
        layout()->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        m_label = new QLabel(name, this);
        m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_label->adjustSize();
        layout()->addWidget(m_label);

        m_edit = new QLineEdit(this);

        m_edit->setPlaceholderText(placeHolder);
        m_edit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        m_edit->setBaseSize(80, 25);
        m_edit->setText(defaultValue);
        layout()->addWidget(m_edit);

        connect(m_edit, &QLineEdit::editingFinished, [this]() { emit EditingFinished(m_edit->text()); });
    }

    QString GetValue() const
    {
        return m_edit->text();
    }

signals:
    void EditingFinished(const QString &value);

protected:
    QLabel *m_label;
    QLineEdit *m_edit;
};
} // namespace editor
