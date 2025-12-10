//
// Created by gorev on 09.10.2025.
//


#include "console_messages_widget.h"

#include "EditorSink.h"
#include "ui_console_messages_widget.h"

#include <QScrollBar>

namespace editor
{
console_messages_widget::console_messages_widget(QWidget *parent)
    : QPlainTextEdit(parent)
    , ui(new Ui::console_messages_widget)
{
    ui->setupUi(this);
    setReadOnly(true);
    setMaximumBlockCount(1000);
}

console_messages_widget::~console_messages_widget()
{
    delete ui;
}


void console_messages_widget::AppendMessage(const LogMessage &message)
{
    BLAINN_PROFILE_FUNC();

    //QMutexLocker locker(&m_mutex);

    QString qMessage = QString::fromStdString(message.message);
    QPalette palette = this->palette();

    QColor color;
    switch (message.level)
    {
    case spdlog::level::trace:
        color = palette.color(QPalette::Disabled, QPalette::Text).lighter(130);
        break;
    case spdlog::level::debug:
        color = palette.color(QPalette::Text);
        break;
    case spdlog::level::info:
        color = QColor(Qt::cyan).lighter(130);
        break;
    case spdlog::level::warn:
        color = QColor(Qt::yellow);
        break;
    case spdlog::level::err:
        color = QColor(Qt::red);
        break;
    case spdlog::level::critical:
        color = QColor(Qt::red);
        break;
    default:
        color = palette.color(QPalette::Text);
        break;
    }

    QTextCharFormat format;
    format.setForeground(color);

    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(qMessage, format);
    // cursor.insertBlock();

    this->setTextCursor(cursor);
    this->ensureCursorVisible();
}


void console_messages_widget::ClearConsole()
{
    clear();
}
} // namespace editor
