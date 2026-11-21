#include <string>
#include<map>
#include<chrono>
#include<ctime>
#include<cstdint>
#include<memory>
#include<logging.hpp>

using namespace std;

class Sink {
    public:
    virtual void write();
    virtual ~Sink() = default;
};

