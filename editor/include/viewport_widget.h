//
// Created by gorev on 02.12.2025.
//

#pragma once
#include "Input/InputSubsystem.h"
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
        setFocusPolicy(Qt::ClickFocus);
        setMouseTracking(false);
    }

protected:

    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        BF_DEBUG("Resized!");

        Blainn::RenderSubsystem::GetInstance().OnResize(event->size().width(), event->size().height());
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        Blainn::Input::UpdateKeyState(static_cast<Blainn::KeyCode>(event->key()), Blainn::KeyState::Pressed);
        QWidget::keyPressEvent(event);
    }

    void keyReleaseEvent(QKeyEvent *event) override
    {
        Blainn::Input::UpdateKeyState(static_cast<Blainn::KeyCode>(event->key()), Blainn::KeyState::Released);
        QWidget::keyReleaseEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        QPoint globalPos = QCursor::pos();
        Blainn::Input::ResetMousePosition(globalPos.x(), globalPos.y());

        Blainn::Input::UpdateButtonState(static_cast<Blainn::MouseButton>(event->button()),
                                         Blainn::ButtonState::Pressed);

        if (event->button() == Qt::MouseButton::LeftButton)
        {
            uint32_t xPos = event->x();
            uint32_t yPos = event->y();
            Blainn::Engine::GetSelectionManager().SelectAt(xPos, yPos);
        }

        QWidget::mousePressEvent(event);
        setFocus();
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        QPoint globalPos = QCursor::pos();
        Blainn::Input::UpdateMousePosition(globalPos.x(), globalPos.y());
        QWidget::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        QPoint globalPos = QCursor::pos();
        Blainn::Input::ResetMousePosition(globalPos.x(), globalPos.y());

        Blainn::Input::UpdateButtonState(static_cast<Blainn::MouseButton>(event->button()),
                                         Blainn::ButtonState::Released);
        QWidget::mouseReleaseEvent(event);
    }
};
