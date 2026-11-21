#include<chrono>
#include<filesystem>
#include<iostream>
#include<memory>
#include<string>
#include<thread>
#include<fileSink.hpp>
#include "logger.cpp"

using namespace std;

int main() {
    filesystem::create_directories("logs");

    registerSink("file", [](const SinkConfig& config) -> shared_ptr<Sink> {
        return make_shared<FileSink>(config);
    });

    SinkConfig fileConfig;
    fileConfig.loglevels = {LogLevel::DEBUG, LogLevel::INFO, LogLevel::WARN, LogLevel::ERROR, LogLevel::FATAL};
    fileConfig.details["path"] = "logs/application.log";
    fileConfig.details["maxFileSize"] = "1024";

    auto options = make_shared<LoggerOptions>();
    options->timeFormat = "ddmmyyhhmmss";
    options->sinks["file"] = fileConfig;

    auto logger = Logger::getInstance(options);

    const LogLevel levels[] = {LogLevel::DEBUG, LogLevel::INFO, LogLevel::WARN, LogLevel::ERROR, LogLevel::FATAL};
    for (int i = 1; i <= 100; i++) {
        logger->log(MessageBase{"message " + to_string(i), levels[i % 5], "main"});
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    cout << "Wrote 100 messages to logs/application.log\n";
    cout << "Rotated files are logs/application.log.<timestamp>.gz and fully compressed on complete.\n";
    return 0;
}
