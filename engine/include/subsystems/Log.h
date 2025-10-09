//
// Created by WhoLeb on 22-Sep-25.
//
#pragma once

#include "spdlog/sinks/base_sink.h"
#include "spdlog/spdlog.h"

#define BLAINN_DEFAULT_LOGGER_NAME "BLAINN"

#ifdef _DEBUG
#define BF_TRACE(...)   if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->trace(__VA_ARGS__); }
#define BF_DEBUG(...)   if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->debug(__VA_ARGS__); }
#define BF_INFO(...)    if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__); }
#define BF_WARN(...)    if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__); }
#define BF_ERROR(...)   if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__); }
#define BF_FATAL(...)   if (spdlog::get(BLAINN_DEFAULT_LOGGER_NAME) != nullptr) { spdlog::get(BLAINN_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__); }
#elif
#define BF_TRACE(...)   (void)0
#define BF_DEBUG(...)   (void)0
#define BF_INFO(...)    (void)0
#define BF_WARN(...)    (void)0
#define BF_ERROR(...)   (void)0
#define BF_FATAL(...)   (void)0
#endif

namespace Blainn
{
    class Log
    {
    public:
        static void Init();
        static void Destroy();
        static void AddSink(const std::shared_ptr<spdlog::sinks::base_sink<std::mutex>>& sink, const std::string & pattern = "%^[%Y-%m-%d %H:%M:%S.%e] %v%$");

    private:
        Log() = default;
        ~Log() = default;

        inline static std::shared_ptr<spdlog::logger> s_Logger{nullptr};
    };
}

