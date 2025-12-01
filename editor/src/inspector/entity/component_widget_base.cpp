//
// Created by gorev on 16.11.2025.
//

#include "entity/component_widget_base.h"

#include "LabelsUtils.h"
#include "scene/EntityTemplates.h"
#include <QLabel>
#include <QMenu>

#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

namespace editor
{
component_widget_base::component_widget_base(const Blainn::Entity &entity, const QString &name, QWidget *parent)
    : themed_panel(parent)
    , m_entity(entity)
{
    setLayout(new QVBoxLayout());
    layout()->setSpacing(5);
    layout()->setContentsMargins(15, 15, 15, 15);
    layout()->setAlignment(Qt::AlignLeft);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *labelLayout = new QHBoxLayout();
    labelLayout->setSpacing(5);
    labelLayout->setAlignment(Qt::AlignLeft);

    m_label = new QLabel(ToHeader3(name), this);
    m_label->setTextFormat(Qt::MarkdownText);
    labelLayout->addWidget(m_label);

    m_settingsButton = new QPushButton("", this);
    m_settingsButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_settingsButton->setFixedSize(20, 20);
    m_settingsButton->setFlat(true);
    m_settingsButton->setIcon(QIcon(":/icons/settings.png"));
    connect(m_settingsButton, &QPushButton::clicked, this, &component_widget_base::ShowSettingsMenu);
    labelLayout->addWidget(m_settingsButton);
    layout()->addItem(labelLayout);

    auto separator = new QFrame(this);
    separator->setFrameShape(HLine);
    layout()->addWidget(separator);

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, [this]() { this->OnUpdate(); });
    m_updateTimer->start(16);
}


void component_widget_base::OnUpdate()
{
}


void component_widget_base::ShowSettingsMenu()
{
    m_settingsMenu = new QMenu(nullptr);

    QAction *deleteAction = m_settingsMenu->addAction("Delete");
    connect(deleteAction, &QAction::triggered, this, [this]() { this->DeleteComponent(); });

    connect(m_settingsMenu, &QMenu::aboutToHide, m_settingsMenu, &QMenu::deleteLater);

    QPoint pos = m_settingsButton->mapToGlobal(m_settingsButton->rect().bottomRight());

    m_settingsMenu->popup(pos);
}
} // namespace editor