#include <string>
#include<map>
#include<chrono>
#include<ctime>
#include<cstdint>
#include<memory>
#include<iomanip>
#include<utils.hpp>

int64_t millisecondsSinceEpoch() {
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
}

std::pair<bool,std::tm> toUtcTime(int64_t millisecs) {
    const std::time_t secs = millisecs / 1000;
    std::tm date{};
#ifdef _WIN32
    const bool ok = gmtime_s(&date, &secs) == 0;
#else
    const bool ok = gmtime_r(&secs, &date) != nullptr;
#endif
    return {ok, date};
}

std::string toString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
    }
    return "?";
}
