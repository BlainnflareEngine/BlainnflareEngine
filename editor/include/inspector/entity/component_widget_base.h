//
// Created by gorev on 10.02.2026.
//

#pragma once
#include "LabelsUtils.h"
#include "Profiler.h"
#include "themed_panel.h"
#include "scene/Entity.h"

#include <QPushButton>
#include <QTimer>
#include <QLabel>
#include <QMenu>
#include <QLayout>


namespace editor
{
class component_widget_base : public themed_panel
{
    Q_OBJECT

public:
    explicit component_widget_base(const Blainn::Entity &entity, const QString &name, QWidget *parent = nullptr);
    virtual ~component_widget_base() = default;

    virtual void OnUpdate();
    virtual void DeleteComponent() = 0;
    virtual void ReportPrefabChanged() = 0;

signals:
    void ComponentChanged();

protected:
    Blainn::Entity m_entity;

    QTimer *m_updateTimer = nullptr;
    QPushButton *m_settingsButton = nullptr;
    QMenu *m_settingsMenu = nullptr;
    QLabel *m_label = nullptr;

    void ShowSettingsMenu();
    void paintEvent(QPaintEvent *event) override;
};


inline component_widget_base::component_widget_base(const Blainn::Entity &entity, const QString &name, QWidget *parent)
    : themed_panel(FOURTH, parent)
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
    separator->setFrameShape(QFrame::HLine);
    layout()->addWidget(separator);

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, [this]() { this->OnUpdate(); });
    m_updateTimer->start(16);
}

inline void component_widget_base::OnUpdate()
{
}

inline void component_widget_base::ShowSettingsMenu()
{
    m_settingsMenu = new QMenu(this);

    QAction *deleteAction = m_settingsMenu->addAction("Delete");
    connect(deleteAction, &QAction::triggered, this, [this]() { this->DeleteComponent(); });

    connect(m_settingsMenu, &QMenu::aboutToHide, m_settingsMenu, &QMenu::deleteLater);

    QPoint pos = m_settingsButton->mapToGlobal(m_settingsButton->rect().bottomRight());
    m_settingsMenu->popup(pos);
}

inline void component_widget_base::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();
    themed_panel::paintEvent(event);
}
} // namespace editor
