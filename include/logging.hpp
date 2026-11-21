#include <string>
#include<cstdint>
#include<memory>

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
    string timeFormat;
    vector<LogLevel> loglevels;
    map<string,string> details;
};

class TimeFormat {
    public:
    string toString(int64_t millisecs);
};

class Message {
    public:
    std::string content;
    LogLevel level;
    std::string sourceNamespace;
};

class Sink {
    public:
    virtual bool write();
};


void registerTimeFormats();
std::shared_ptr<TimeFormat> getTimeFormat(std::string format);

void registerSinks();
std::shared_ptr<Sink> getSink(SinkType sinkType);
