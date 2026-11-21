#include<map>
#include<string>
#include<stdexcept>
#include<vector>
#include<logging.hpp>
#include<utils.hpp>

using namespace std;

class LoggerOptions {
    public:
    string timeFormat;
    map<SinkType,SinkConfig> sinks;
};

class Logger {
    private:
    shared_ptr<LoggerOptions> loggerOptionsPtr;
    shared_ptr<TimeFormat> timeFormat;
    map<LogLevel,shared_ptr<Sink>> logRoutes;

    Logger(shared_ptr<LoggerOptions> loggerOptionsPtr): loggerOptionsPtr(loggerOptionsPtr) {
         if (!loggerOptionsPtr) {
            throw std::invalid_argument("Logger options are required");
        }

        registerTimeFormats();
        registerSinks();

        timeFormat = getTimeFormat(loggerOptionsPtr->timeFormat);
        if (!timeFormat) {
            throw std::invalid_argument("Unsupported time format: " + loggerOptionsPtr->timeFormat);
        }

        for (auto it=loggerOptionsPtr->sinks.begin();it!=loggerOptionsPtr->sinks.end();it++) {
            auto sinkType = it->first;
            auto levels = it->second.loglevels;

            auto sink = getSink(sinkType, it->second);
            if (!sink) {
                throw std::invalid_argument("Unknown sink type: " + sinkType);
            }

            for(auto level:levels) {
                if (logRoutes.find(level)!=logRoutes.end()) {
                    throw std::invalid_argument("message level should be tied to a sink");
                }
                logRoutes[level] = sink;
            }
        }
    };
    public:
    static shared_ptr<Logger> getInstance(shared_ptr<LoggerOptions> sptr){
        static auto logger = shared_ptr<Logger>(new Logger(sptr));
        return logger;
    }
    void log(const MessageBase& messageBase){
        Message message(messageBase, timeFormat->toString(millisecondsSinceEpoch()));

        auto route = logRoutes.find(message.level);
        if (route == logRoutes.end()) return;

        route->second->write(message);
    }
};
