#pragma once

#include<mutex>
#include<logging.hpp>

class ConsoleSink: public Sink {
    public:
    bool write(const Message& message) override;

    private:
    std::mutex consoleMutex;
};
