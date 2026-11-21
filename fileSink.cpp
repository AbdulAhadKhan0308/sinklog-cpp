#include <string>
#include<stdexcept>
#include<fileSink.hpp>

using namespace std;

static string toString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
    }
    return "UNKNOWN";
}

FileSink::FileSink(const SinkConfig& config) {
    auto path = config.details.find("path");
    if (path == config.details.end() || path->second.empty()) {
        throw invalid_argument("File sink requires a 'path' in SinkConfig.details");
    }

    file.open(path->second, ios::app);
    if (!file) {
        throw runtime_error("Unable to open log file: " + path->second);
    }
}

bool FileSink::write(const Message& message) {
    lock_guard<mutex> lock(fileMutex);
    file << "[" << toString(message.level) << "] "
         << message.sourceNamespace << ": "
         << message.content << '\n';
    file.flush();
    return static_cast<bool>(file);
}
