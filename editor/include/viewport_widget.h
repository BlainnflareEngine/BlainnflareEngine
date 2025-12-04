//
// Created by gorev on 02.12.2025.
//

#pragma once
#include "Log.h"
#include "RenderSubsystem.h"


#include <QResizeEvent>
#include <QWidget>

class viewport_widget : public QWidget
{
    Q_OBJECT

public:
    explicit viewport_widget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        BF_DEBUG("Resized!");

        Blainn::RenderSubsystem::GetInstance().OnResize(event->size().width(), event->size().height());
    }
};
