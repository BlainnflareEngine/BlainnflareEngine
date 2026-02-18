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
    do                                                                                                                 \
    {                                                                                                                  \
        auto bf_logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);                                                     \
        if (bf_logger != nullptr)                                                                                      \
        {                                                                                                              \
            bf_logger->trace(__VA_ARGS__);                                                                             \
        }                                                                                                              \
    } while (0)
#define BF_DEBUG(...)                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        auto bf_logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);                                                     \
        if (bf_logger != nullptr)                                                                                      \
        {                                                                                                              \
            bf_logger->debug(__VA_ARGS__);                                                                             \
        }                                                                                                              \
    } while (0)
#define BF_INFO(...)                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        auto bf_logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);                                                     \
        if (bf_logger != nullptr)                                                                                      \
        {                                                                                                              \
            bf_logger->info(__VA_ARGS__);                                                                              \
        }                                                                                                              \
    } while (0)
#define BF_WARN(...)                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        auto bf_logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);                                                     \
        if (bf_logger != nullptr)                                                                                      \
        {                                                                                                              \
            bf_logger->warn(__VA_ARGS__);                                                                              \
        }                                                                                                              \
    } while (0)
#define BF_ERROR(...)                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        auto bf_logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);                                                     \
        if (bf_logger != nullptr)                                                                                      \
        {                                                                                                              \
            bf_logger->error(__VA_ARGS__);                                                                             \
        }                                                                                                              \
    } while (0)
#define BF_FATAL(...)                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        auto bf_logger = spdlog::get(BLAINN_DEFAULT_LOGGER_NAME);                                                     \
        if (bf_logger != nullptr)                                                                                      \
        {                                                                                                              \
            bf_logger->critical(__VA_ARGS__);                                                                          \
        }                                                                                                              \
    } while (0)
#else
#define BF_TRACE(...)                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define BF_DEBUG(...)                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define BF_INFO(...)                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define BF_WARN(...)                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define BF_ERROR(...)                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define BF_FATAL(...)                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
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
