#include<map>
#include<memory>
#include<mutex>
#include<string>
#include<stdexcept>
#include<logging.hpp>
#include<fileSink.hpp>

using namespace std;

using SinkFactories = map<SinkType,SinkFactory>;
SinkFactories sinkFactories;
mutex sinkFactoriesMutex;

void registerSink(const SinkType& sinkType, SinkFactory factory) {
    if (sinkType.empty()) {
        throw invalid_argument("SinkType empty");
    }
    if (!factory) {
        throw invalid_argument("SinkFactory not provided");
    }
    lock_guard<mutex> lock(sinkFactoriesMutex);
    sinkFactories[sinkType] = factory;
}

void registerSinks() {
    lock_guard<mutex> lock(sinkFactoriesMutex);
    sinkFactories.emplace("file", [](const SinkConfig& config) -> shared_ptr<Sink> {
        return make_shared<FileSink>(config);
    });
}

shared_ptr<Sink> getSink(const SinkType& sinkType, const SinkConfig& config) {
    SinkFactory factory;
    {
        lock_guard<mutex> lock(sinkFactoriesMutex);
        auto it = sinkFactories.find(sinkType);
        if (it == sinkFactories.end()) return nullptr;
        factory = it->second;
    }
    return factory(config);
}
