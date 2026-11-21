#include <string>
#include<map>
#include<chrono>
#include<ctime>
#include<cstdint>
#include<memory>
#include<iomanip>
#include<utils.hpp>

int64_t millisecondsSinceEpoch() {
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
}
