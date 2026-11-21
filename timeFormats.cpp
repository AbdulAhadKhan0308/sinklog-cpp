#include <string>
#include<map>
#include<chrono>
#include<ctime>
#include<cstdint>
#include<memory>
#include<iomanip>
#include<logging.hpp>

using namespace std;

int64_t millisecondsSinceEpoch() {
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
}

pair<bool,tm> toUtcTime(int64_t millisecs) {
    time_t secs = millisecs/1000;
    tm date{};
    if(gmtime_s(&date,&secs)!=0) {
            return {0,date};
    }
    return {1,date};
}

using TimeFormats = map<string,shared_ptr<TimeFormat>>;
TimeFormats timeFormats;

void registerTimeFormats(){
    class: public TimeFormat {
        string toString(int64_t millisecs){
            auto date = toUtcTime(millisecs);

            if(date.first==false) return "?";
            
            char buffer[13];
            const auto length =strftime(buffer, sizeof(buffer), "%d%m%y%H%M%S", &(date.second));
            if(length==0) return "?";

            return string(buffer,length);
        }
    } formatter_ddmmyyhhmmss;


    timeFormats["ddmmyyhhmmss"] = make_shared<decltype(formatter_ddmmyyhhmmss)>();

};

shared_ptr<TimeFormat> getTimeFormat(string format){
    if(timeFormats.find(format)==timeFormats.end()) return nullptr;
    return timeFormats[format]; 
}
