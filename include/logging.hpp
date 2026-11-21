#pragma once

#include <string>
#include<cstdint>
#include<functional>
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

using SinkType = std::string;

class SinkConfig {
    public:
    std::vector<LogLevel> loglevels;
    std::map<std::string,std::string> details;
};

class TimeFormat {
    public:
    virtual std::string toString(int64_t millisecs) = 0;
    virtual ~TimeFormat() = default;
};

class MessageBase{
    public:
    std::string content;
    LogLevel level;
    std::string sourceNamespace;
};

class Message: public MessageBase {
    public:
    std::string timestamp;

    Message(const MessageBase& base, const std::string& timestamp)
        : MessageBase(base), timestamp(timestamp) {}
};

class Sink {
    public:
    virtual bool write(const Message& message) = 0;
    virtual ~Sink() = default;
};


void registerTimeFormats();
std::shared_ptr<TimeFormat> getTimeFormat(std::string format);

void registerSinks();
using SinkFactory = std::function<std::shared_ptr<Sink>(const SinkConfig&)>;
void registerSink(const SinkType& sinkType, SinkFactory factory);
std::shared_ptr<Sink> getSink(const SinkType& sinkType, const SinkConfig& config);
