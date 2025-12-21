//
// Created by gorev on 09.10.2025.
//

#pragma once
#include "console_messages_widget.h"
#include "spdlog/sinks/base_sink.h"

#include <qobjectdefs.h>
#include <QMetaType>


namespace editor
{
struct LogMessage
{
    spdlog::level::level_enum level;
    std::string message;
};


Q_DECLARE_METATYPE(LogMessage)

template <typename Mutex> class EditorSink : public spdlog::sinks::base_sink<Mutex>
{
public:
    EditorSink(console_messages_widget *console)
        : m_console(console)
    {
    }

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override
    {
        spdlog::memory_buf_t formatted;
        this->formatter_->format(msg, formatted);
        LogMessage log_msg{msg.level, fmt::to_string(formatted)};
        QMetaObject::invokeMethod(m_console, "AppendMessage", Qt::QueuedConnection, Q_ARG(LogMessage, log_msg));
    }

    void flush_() override
    {
    }

private:
    console_messages_widget *m_console;
};

} // namespace editor