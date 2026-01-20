//
// Created by gorev on 15.11.2025.
//

#pragma once
#include <QFrame>

namespace editor
{

class themed_panel : public QFrame
{
    Q_OBJECT

public:
    enum PanelColor
    {
        FIRST,
        SECOND,
        THIRD,
        FOURTH,
    };

    explicit themed_panel(PanelColor color, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;


private:
    PanelColor m_color;
};

} // namespace editor