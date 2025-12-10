//
// Created by gorev on 07.12.2025.
//

#pragma once
#include "EnumTemplates.h"
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

namespace editor
{

template <typename EnumType> class enum_input_widget : public QWidget
{
public:
    explicit enum_input_widget(const QString &labelText, QWidget *parent = nullptr)
        : QWidget(parent)
        , m_comboBox(new QComboBox(this))
    {
        auto *label = new QLabel(labelText, this);

        const auto &items = enumToStrings<EnumType>();
        for (const auto &[value, text] : items)
        {
            m_comboBox->addItem(text, QVariant::fromValue(static_cast<EnumType>(value)));
        }

        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(10);
        layout->addWidget(label);
        layout->addWidget(m_comboBox, 1);

        setLayout(layout);

        connect(m_comboBox, &QComboBox::currentIndexChanged, this, &enum_input_widget::OnCurrentIndexChanged);
    }

    EnumType GetValue() const
    {
        auto data = m_comboBox->currentData();
        if (!data.isValid()) return EnumType{};
        return static_cast<EnumType>(data.value<EnumType>());
    }

    void SetValue(EnumType value)
    {
        auto underlying = static_cast<EnumType>(value);
        int index = m_comboBox->findData(QVariant::fromValue(underlying));
        if (index != -1)
        {
            m_comboBox->setCurrentIndex(index);
        }
    }

    QComboBox *comboBox() const
    {
        return m_comboBox;
    }

    template <typename EnumType> void ValueChanged(EnumType value)
    {
    }

private:
    void OnCurrentIndexChanged(int)
    {
        EnumType value = GetValue();
        ValueChanged(value);
    }

    QComboBox *m_comboBox;
};


} // namespace editor