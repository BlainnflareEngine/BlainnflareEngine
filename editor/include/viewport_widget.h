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
        setMouseTracking(true);
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);

        Blainn::RenderSubsystem::GetInstance().OnResize(event->size().width(), event->size().height());
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        Blainn::Input::UpdateKeyState(static_cast<Blainn::KeyCode>(event->nativeVirtualKey()),
                                      Blainn::KeyState::Pressed);
        QWidget::keyPressEvent(event);
    }

    void keyReleaseEvent(QKeyEvent *event) override
    {
        Blainn::Input::UpdateKeyState(static_cast<Blainn::KeyCode>(event->nativeVirtualKey()),
                                      Blainn::KeyState::Released);
        QWidget::keyReleaseEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        QPoint globalPos = QCursor::pos();
        Blainn::Input::ResetMousePosition(event->x(), event->y());

        int eventButton = static_cast<int>(event->button());
        Blainn::Input::UpdateButtonState(static_cast<Blainn::MouseButton>(--eventButton),
                                         Blainn::ButtonState::Pressed);
        QWidget::mousePressEvent(event);
        setFocus();
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        QPoint globalPos = QCursor::pos();
        Blainn::Input::UpdateMousePosition(event->x(), event->y());
        QWidget::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        QPoint globalPos = QCursor::pos();
        Blainn::Input::ResetMousePosition(event->x(), event->y());

        int eventButton = static_cast<int>(event->button());
        Blainn::Input::UpdateButtonState(static_cast<Blainn::MouseButton>(--eventButton),
                                         Blainn::ButtonState::Released);
        QWidget::mouseReleaseEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override
    {
        Blainn::Input::UpdateScrollState(event->angleDelta().x(), event->angleDelta().y());
        QWidget::wheelEvent(event);
    }
};
