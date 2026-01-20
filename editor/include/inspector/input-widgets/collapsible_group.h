//
// Created by gorev on 19.01.2026.
//

#pragma once
#include "themed_panel.h"

#include <QToolBar>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>


class QScrollArea;
class QParallelAnimationGroup;
class QToolButton;

namespace editor
{

class collapsible_group : public themed_panel
{
    Q_OBJECT

public:
    explicit collapsible_group(const QString &title, PanelColor color = SECOND, QWidget *parent = nullptr);

    void AddWidget(QWidget *widget);

private slots:
    void OnToggled(bool checked);

private:
    QGridLayout *m_mainLayout = nullptr;
    QToolButton *m_toggleButton = nullptr;
    QFrame *m_headerLine = nullptr;
    QParallelAnimationGroup *m_toggleAnimation = nullptr;
    QScrollArea *m_contentArea = nullptr;
    int m_animationDuration = 400;
};

} // namespace editor