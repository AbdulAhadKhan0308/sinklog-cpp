#include<map>
#include<string>
#include<logging.hpp>
#include<stdexcept>
#include<vector>

using namespace std;
using LoggerOptions = map<SinkType,SinkConfig>;

class Logger {
    private:
    shared_ptr<LoggerOptions> loggerOptionsPtr;
    map<LogLevel,vector<shared_ptr<Sink>>> logRoutes;

    Logger(shared_ptr<LoggerOptions> loggerOptionsPtr): loggerOptionsPtr(loggerOptionsPtr) {
         if (!loggerOptionsPtr) {
            throw std::invalid_argument("Logger options are required");
        }

        registerTimeFormats();
        registerSinks();

        for (auto it=loggerOptionsPtr->begin();it!=loggerOptionsPtr->end();it++) {
            auto sinkType = it->first;
            auto levels = it->second.loglevels;

            for(auto level:levels) {
                logRoutes[level].push_back(getSink(sinkType));
            }
        }
    };
    public:
    shared_ptr<Logger> getInstance(shared_ptr<LoggerOptions> sptr){
        static auto logger = shared_ptr<Logger>(new Logger(sptr));
        return logger;
    }
    void log(Message& message){

    }
};
