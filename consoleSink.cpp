#include<iostream>
#include<string>
#include<consoleSink.hpp>
#include<utils.hpp>

using namespace std;

bool ConsoleSink::write(const Message& message) {
    const string line = message.timestamp + " "
        + "[" + toString(message.level) + "] "
        + message.sourceNamespace + ": "
        + message.content + '\n';

    lock_guard<mutex> lock(consoleMutex);
    cout << line << flush;
    return static_cast<bool>(cout);
}
