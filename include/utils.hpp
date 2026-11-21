#pragma once

#include<cstdint>
#include<ctime>
#include<string>
#include<utility>
#include<logging.hpp>

int64_t millisecondsSinceEpoch();
std::pair<bool,std::tm> toUtcTime(int64_t millisecs);
std::string toString(LogLevel level);
