#pragma once

#include<condition_variable>
#include<cstdint>
#include<fstream>
#include<mutex>
#include<queue>
#include<string>
#include<thread>
#include<logging.hpp>

class FileSink: public Sink {
    public:
    explicit FileSink(const SinkConfig& config);
    ~FileSink() override;
    bool write(const Message& message) override;

    private:
    void openFile();
    void rotate();
    void queueLeftoverRotations();
    void compressPending();

    std::string path;
    uint64_t maxFileSize = 0;
    uint64_t currentSize = 0;
    std::ofstream file;
    std::mutex fileMutex;

    std::queue<std::string> pendingCompression;
    bool stopping = false;
    std::mutex compressionMutex;
    std::condition_variable compressionReady;
    std::thread compressor;
};
