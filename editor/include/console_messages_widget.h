//
// Created by gorev on 09.10.2025.
//

#pragma once

#include <QMutex>
#include <QPlainTextEdit>

namespace editor
{
struct LogMessage;
}
namespace editor
{
QT_BEGIN_NAMESPACE
namespace Ui
{
class console_messages_widget;
}
QT_END_NAMESPACE

class console_messages_widget : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit console_messages_widget(QWidget *parent = nullptr);
    ~console_messages_widget() override;

public slots:
    void AppendMessage(const LogMessage &message);

    void ClearConsole();

private:
    Ui::console_messages_widget *ui;

    QMutex m_mutex;
};
} // namespace editor
