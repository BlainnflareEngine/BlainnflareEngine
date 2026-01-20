//
// Created by gorev on 20.01.2026.
//

#pragma once
#include "FileSystemUtils.h"
#include "LabelsUtils.h"
#include "themed_panel.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "oclero/qlementine/widgets/Label.hpp"
#include "oclero/qlementine/widgets/LineEdit.hpp"

#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>


namespace editor
{
class string_list_input : public themed_panel
{
    Q_OBJECT
public:
    explicit string_list_input(const QString &label, const QString &placeHolder = "",
                               const eastl::vector<eastl::string> &initialValues = {}, QWidget *parent = nullptr)
        : themed_panel(FOURTH, parent)
    {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(5);

        m_label = new QLabel(ToHeader4(label), this);
        m_label->setTextFormat(Qt::MarkdownText);
        mainLayout->addWidget(m_label);


        m_listWidget = new QListWidget(this);
        m_listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_listWidget->setFixedHeight(60);
        mainLayout->addWidget(m_listWidget);

        auto *controlLayout = new QHBoxLayout();
        controlLayout->setContentsMargins(0, 0, 0, 0);
        controlLayout->setSpacing(5);

        m_inputField = new QLineEdit(this);
        m_inputField->setPlaceholderText(placeHolder);
        controlLayout->addWidget(m_inputField);

        m_addButton = new QPushButton("Add", this);
        m_removeButton = new QPushButton("Remove", this);
        m_removeButton->setEnabled(false);

        controlLayout->addWidget(m_addButton);
        controlLayout->addWidget(m_removeButton);
        mainLayout->addLayout(controlLayout);

        SetValue(initialValues);

        connect(m_addButton, &QPushButton::clicked, this, &string_list_input::OnAddClicked);
        connect(m_removeButton, &QPushButton::clicked, this, &string_list_input::OnRemoveClicked);
        connect(m_listWidget, &QListWidget::itemSelectionChanged, this, [this]() { UpdateButtonsState(); });
        connect(m_listWidget, &QListWidget::itemChanged, this, &string_list_input::OnItemChanged);
        connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &string_list_input::OnItemDoubleClicked);
        connect(m_inputField, &QLineEdit::returnPressed, this, &string_list_input::OnAddClicked);
    }

    eastl::vector<eastl::string> GetValue() const
    {
        eastl::vector<eastl::string> result;
        for (int i = 0; i < m_listWidget->count(); ++i)
        {
            QString text = m_listWidget->item(i)->text();
            if (!text.trimmed().isEmpty())
            {
                result.push_back(ToEASTLString(text));
            }
        }
        return result;
    }

    void SetValue(const eastl::vector<eastl::string> &values)
    {
        m_listWidget->clear();
        for (const auto &str : values)
        {
            if (!str.empty())
            {
                auto *item = new QListWidgetItem(ToQString(str), m_listWidget);
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            }
        }
        emit ValueChanged();
    }

signals:
    void ValueChanged();

private slots:
    void OnAddClicked()
    {
        QString text = m_inputField->text().trimmed();
        if (!text.isEmpty())
        {
            auto *item = new QListWidgetItem(text, m_listWidget);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            m_inputField->clear();
            emit ValueChanged();
        }
    }

    void OnRemoveClicked()
    {
        auto selectedItems = m_listWidget->selectedItems();
        for (auto *item : selectedItems)
        {
            delete m_listWidget->takeItem(m_listWidget->row(item));
        }
        emit ValueChanged();
    }

    void OnItemChanged(QListWidgetItem *item)
    {
        emit ValueChanged();
    }

    void OnItemDoubleClicked(QListWidgetItem *item)
    {
        m_listWidget->editItem(item);
    }

private:
    void UpdateButtonsState()
    {
        m_removeButton->setEnabled(!m_listWidget->selectedItems().empty());
    }

    QLabel *m_label = nullptr;
    QListWidget *m_listWidget = nullptr;
    QLineEdit *m_inputField = nullptr;
    QPushButton *m_addButton = nullptr;
    QPushButton *m_removeButton = nullptr;
};
} // namespace editor
