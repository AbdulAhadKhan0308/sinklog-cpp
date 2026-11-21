#include<map>
#include<memory>
#include<functional>
#include<logging.hpp>
#include<fileSink.hpp>

using namespace std;

using SinkFactory = function<shared_ptr<Sink>(const SinkConfig&)>;
using SinkFactories = map<SinkType,SinkFactory>;
SinkFactories sinkFactories;

void registerSinks() {
    sinkFactories[SinkType::FILE] = [](const SinkConfig& config) -> shared_ptr<Sink> {
        return make_shared<FileSink>(config);
    };
}

shared_ptr<Sink> getSink(SinkType sinkType, const SinkConfig& config) {
    auto factory = sinkFactories.find(sinkType);
    if (factory == sinkFactories.end()) return nullptr;
    return factory->second(config);
}
