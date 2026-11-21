#pragma once

#include <string>
#include<cstdint>
#include<map>
#include<memory>
#include<vector>

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

enum class SinkType {
    FILE,
    CONSOLE,
    DB
};

class SinkConfig {
    public:
    std::string timeFormat;
    std::vector<LogLevel> loglevels;
    std::map<std::string,std::string> details;
};

class TimeFormat {
    public:
    std::string toString(int64_t millisecs);
};

class Message {
    public:
    std::string content;
    LogLevel level;
    std::string sourceNamespace;
};

class Sink {
    public:
    virtual bool write(const Message& message) = 0;
    virtual ~Sink() = default;
};


void registerTimeFormats();
std::shared_ptr<TimeFormat> getTimeFormat(std::string format);

void registerSinks();
std::shared_ptr<Sink> getSink(SinkType sinkType, const SinkConfig& config);
