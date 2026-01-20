//
// Created by gorev on 19.01.2026.
//

#include "input-widgets/collapsible_group.h"


#include "entity_inspector_content.h"
#include "input-widgets/collapsible_group.h"

#include "oclero/qlementine/style/QlementineStyle.hpp"

#include <QToolButton>
#include <QTimer>
#include <QStyle>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QParallelAnimationGroup>

namespace editor
{
collapsible_group::collapsible_group(const QString &title, PanelColor color, QWidget *parent)
    : themed_panel(color, parent)
{
    m_mainLayout = new QGridLayout(this);
    m_toggleButton = new QToolButton(this);
    m_headerLine = new QFrame(this);
    m_contentArea = new QScrollArea(this);
    m_toggleAnimation = new QParallelAnimationGroup(this);

    QString textColor = "#ffffff";
    if (auto *style = oclero::qlementine::appStyle())
    {
        textColor = style->theme().secondaryColor.name();
    }

    m_toggleButton->setStyleSheet(QString("QToolButton {"
                                          "   border: none;"
                                          "   text-align: left;"
                                          "   padding: 4px;"
                                          "   color: %1;"
                                          "   background: transparent;"
                                          "}"
                                          "QToolButton::menu-indicator {"
                                          "   image: none;"
                                          "}")
                                      .arg(textColor));
    m_toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toggleButton->setArrowType(Qt::RightArrow);
    m_toggleButton->setText(title);
    m_toggleButton->setCheckable(true);
    m_toggleButton->setChecked(false);

    m_headerLine->setFrameShape(HLine);
    m_headerLine->setFrameShadow(Sunken);
    m_headerLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_contentArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_contentArea->setMaximumHeight(0);
    m_contentArea->setMinimumHeight(0);
    m_contentArea->setWidgetResizable(true);
    m_contentArea->viewport()->setAutoFillBackground(false);

    auto *contentWidget = new QWidget(m_contentArea);
    auto *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(20, 5, 5, 5);
    contentLayout->setSpacing(5);
    m_contentArea->setWidget(contentWidget);

    auto anim1 = new QPropertyAnimation(this, "minimumHeight");
    anim1->setEasingCurve(QEasingCurve::OutQuad);
    auto anim2 = new QPropertyAnimation(this, "maximumHeight");
    anim2->setEasingCurve(QEasingCurve::OutQuad);
    auto anim3 = new QPropertyAnimation(m_contentArea, "maximumHeight");
    anim3->setEasingCurve(QEasingCurve::OutQuad);

    m_toggleAnimation->addAnimation(anim1);
    m_toggleAnimation->addAnimation(anim2);
    m_toggleAnimation->addAnimation(anim3);

    m_mainLayout->setVerticalSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    int row = 0;
    m_mainLayout->addWidget(m_toggleButton, row, 0, 1, 1, Qt::AlignLeft);
    m_mainLayout->addWidget(m_headerLine, row++, 1, 1, 1);
    m_mainLayout->addWidget(m_contentArea, row, 0, 1, 2);
    setLayout(m_mainLayout);
    connect(m_toggleButton, &QToolButton::toggled, this, &collapsible_group::OnToggled);
}


void collapsible_group::AddWidget(QWidget *widget)
{
    if (!widget) return;

    auto *contentWidget = m_contentArea->widget();
    auto *layout = qobject_cast<QVBoxLayout *>(contentWidget->layout());
    if (layout)
    {
        layout->addWidget(widget);

        int collapsedHeight = sizeHint().height() - m_contentArea->maximumHeight();
        int contentHeight = layout->sizeHint().height();

        for (int i = 0; i < m_toggleAnimation->animationCount(); ++i)
        {
            auto *anim = static_cast<QPropertyAnimation *>(m_toggleAnimation->animationAt(i));
            anim->setDuration(m_animationDuration);
            if (i < 2)
            {
                anim->setStartValue(collapsedHeight);
                anim->setEndValue(collapsedHeight + contentHeight);
            }
            else
            {
                anim->setStartValue(0);
                anim->setEndValue(contentHeight);
            }
        }
    }
}


void collapsible_group::OnToggled(bool checked)
{
    m_toggleButton->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);
    m_toggleAnimation->setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    m_toggleAnimation->start();
}
} // namespace editor