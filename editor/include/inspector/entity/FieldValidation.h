//
// Created by gorev on 15.11.2025.
//

#pragma once
#include <QKeyEvent>
#include <QLineEdit>


namespace editor
{
inline bool ValidateFloat(const QLineEdit *lineEdit, const QKeyEvent *keyEvent)
{
    if (lineEdit)
    {
        QString currentText = lineEdit->text();
        int cursorPos = lineEdit->cursorPosition();

        switch (keyEvent->key())
        {
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
            return false;

        case Qt::Key_Minus:
        case Qt::Key_Underscore:
            if (cursorPos == 0 && !currentText.contains('-')) return false;
            else return true;

        case Qt::Key_Period:
            if (!currentText.contains('.')) return false;
            else return true;

        case Qt::Key_Comma:
            if (!currentText.contains('.')) lineEdit->text().insert(cursorPos, '.');
            return true;

        case Qt::Key_Backspace:
        case Qt::Key_Delete:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Tab:
        case Qt::Key_Escape:
            return false;

        default:
            return true;
        }
    }
}
} // namespace editor
