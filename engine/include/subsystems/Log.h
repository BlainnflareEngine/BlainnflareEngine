//
// Created by WhoLeb on 22-Sep-25.
//
#pragma once

#include "spdlog/sinks/base_sink.h"
#include "spdlog/spdlog.h"

#include "aliases.h"

#define BLAINN_DEFAULT_LOGGER_NAME "BLAINN"

#if defined(_DEBUG) || defined(BLAINN_HAS_CONSOLE)
#define BF_TRACE(...)                                                                                                  \
    if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr)                                                            \
    {                                                                                                                  \
        spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->trace(__VA_ARGS__);                                                   \
    }
#define BF_DEBUG(...)                                                                                                  \
    if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr)                                                            \
    {                                                                                                                  \
        spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->debug(__VA_ARGS__);                                                   \
    }
#define BF_INFO(...)                                                                                                   \
    if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr)                                                            \
    {                                                                                                                  \
        spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__);                                                    \
    }
#define BF_WARN(...)                                                                                                   \
    if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr)                                                            \
    {                                                                                                                  \
        spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__);                                                    \
    }
#define BF_ERROR(...)                                                                                                  \
    if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr)                                                            \
    {                                                                                                                  \
        spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__);                                                   \
    }
#define BF_FATAL(...)                                                                                                  \
    if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr)                                                            \
    {                                                                                                                  \
        spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__);                                                \
    }
#else
#define BF_TRACE(...) ;
#define BF_DEBUG(...) ;
#define BF_INFO(...) ;
#define BF_WARN(...) ;
#define BF_ERROR(...) ;
#define BF_FATAL(...) ;
#endif

namespace Blainn
{
class Log
{
public:
    static void Init();
    static void Destroy();
    static void AddSink(const std::shared_ptr<spdlog::sinks::base_sink<std::mutex>> &sink,
                        const std::string &pattern = "%^[%Y-%m-%d %H:%M:%S.%e] %v%$");
    static void RemoveSink(const std::shared_ptr<spdlog::sinks::base_sink<std::mutex>> &sink);

private:
    Log() = default;
    ~Log() = default;

    inline static std::shared_ptr<spdlog::logger> s_Logger{nullptr};
};
} // namespace Blainn

inline std::ostream &operator<<(std::ostream &os, const Blainn::Vec2 &v)
{
    return os << std::format("{:6f} {:6f}\n", v.x, v.y);
}

inline std::ostream &operator<<(std::ostream &os, const Blainn::Vec3 &v)
{
    return os << std::format("{:6f} {:6f} {:6f}\n", v.x, v.y, v.z);
}


inline std::ostream &operator<<(std::ostream &os, const Blainn::Vec4 &v)
{
    return os << std::format("{:6f} {:6f} {:6f} {:6f}\n", v.x, v.y, v.z, v.w);
}

inline std::ostream &operator<<(std::ostream &os, const Blainn::Quat &q)
{
    return os << std::format("{:6f} {:6f} {:6f} {:6f}\n", q.x, q.y, q.z, q.w);
}
