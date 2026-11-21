#include<algorithm>
#include<cctype>
#include<iostream>
#include<memory>
#include<string>
#include<vector>
#include "../logger.cpp"

using namespace std;

static int failures = 0;

static void check(bool condition, const string& description) {
    if (!condition) {
        cerr << "FAIL: " << description << '\n';
        failures++;
    }
}

class MemorySink: public Sink {
    public:
    vector<Message> messages;

    bool write(const Message& message) override {
        messages.push_back(message);
        return true;
    }
};

int main() {
    auto memorySink = make_shared<MemorySink>();
    registerSink("memory", [memorySink](const SinkConfig&) -> shared_ptr<Sink> {
        return memorySink;
    });

    auto options = make_shared<LoggerOptions>();
    options->timeFormat = "ddmmyyhhmmss";
    SinkConfig config;
    config.loglevels = {LogLevel::WARN};
    options->sinks["memory"] = config;

    auto logger = Logger::getInstance(options);
    logger->log(MessageBase{"disk almost full", LogLevel::WARN, "storage"});
    logger->log(MessageBase{"not routed", LogLevel::INFO, "storage"});

    check(memorySink->messages.size() == 1, "only the WARN message reaches the WARN sink");
    if (memorySink->messages.size() == 1) {
        const auto& message = memorySink->messages[0];
        check(message.content == "disk almost full", "content is passed through");
        check(message.level == LogLevel::WARN, "level is passed through");
        check(message.sourceNamespace == "storage", "source namespace is passed through");
        check(message.timestamp.size() == 12,"timestamp is added in ddmmyyhhmmss format");
    }

    if (failures > 0) return 1;
    cout << "PASS: logger routes by level and adds a timestamp\n";
    return 0;
}
