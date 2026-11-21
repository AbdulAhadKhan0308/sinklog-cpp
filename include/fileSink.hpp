#pragma once

#include<fstream>
#include<mutex>
#include<logging.hpp>

class FileSink: public Sink {
    public:
    explicit FileSink(const SinkConfig& config);
    bool write(const Message& message) override;

    private:
    std::ofstream file;
    std::mutex fileMutex;
};
