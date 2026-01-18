//
// Created by admin on 22-Sep-25.
//

#include "subsystems/Log.h"

#include "EASTL/shared_ptr.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>

#include <filesystem>

namespace Blainn
{
    void Log::Init()
    {
        std::string logsDirectory = "logs";
        if (!std::filesystem::exists(logsDirectory))
            std::filesystem::create_directory(logsDirectory);


        std::vector<spdlog::sink_ptr> sinks {
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/BLAINN.log", true),
#ifdef BLAINN_HAS_CONSOLE
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
#endif
        };
        sinks[0]->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] %v%$");
#ifdef BLAINN_HAS_CONSOLE
        sinks[1]->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] %v%$");
#endif

        s_Logger = std::make_shared<spdlog::logger>("BLAINN", sinks.begin(), sinks.end());
        s_Logger->set_level(spdlog::level::trace);
        s_Logger->flush_on(spdlog::level::trace);
        spdlog::register_logger(s_Logger);
    }

    void Log::Destroy()
    {
        s_Logger.reset();
        spdlog::drop_all();
        spdlog::shutdown();
    }


    void Log::AddSink(const std::shared_ptr<spdlog::sinks::base_sink<std::mutex>> &sink, const std::string &pattern)
    {
        s_Logger->sinks().push_back(sink);
        s_Logger->sinks().back()->set_pattern(pattern);
    }

    void Log::RemoveSink(const std::shared_ptr<spdlog::sinks::base_sink<std::mutex>> &sink)
    {
        auto& sinks = s_Logger->sinks();
        const auto& sinkit = std::find(sinks.begin(), sinks.end(), sink);
        if (sinkit != sinks.end())
            s_Logger->sinks().erase(sinkit);
    }
}

