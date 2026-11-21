#include <string>
#include<algorithm>
#include<cctype>
#include<ctime>
#include<filesystem>
#include<iomanip>
#include<sstream>
#include<stdexcept>
#include<zlib.h>
#include<fileSink.hpp>
#include<utils.hpp>

using namespace std;

static string toString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
    }
    return "?";
}

static bool isRotationTimestamp(const string& value) {
    return !value.empty() && all_of(value.begin(), value.end(), [](unsigned char c) { return isdigit(c) || c == '-'; });
}

static string rotationTimestamp() {
    const auto millisecs = millisecondsSinceEpoch();
    const time_t secs = millisecs / 1000;
    tm date{};
    if (gmtime_s(&date, &secs) != 0) return to_string(millisecs);

    ostringstream out;
    out << put_time(&date, "%Y%m%d-%H%M%S") << '-' << setw(3) << setfill('0') << millisecs % 1000;
    return out.str();
}

static bool gzipFile(const string& source) {
    ifstream in(source, ios::binary);
    if (!in) return false;

    const string target = source + ".gz";
    gzFile out = gzopen(target.c_str(), "wb");
    if (!out) return false;

    bool ok = true;
    char buffer[64 * 1024];
    while (in) {
        in.read(buffer, sizeof(buffer));
        const auto count = in.gcount();
        if (count == 0) break;
        if (gzwrite(out, buffer, static_cast<unsigned>(count)) != count) {
            ok = false;
            break;
        }
    }
    if (in.bad()) ok = false;
    if (gzclose(out) != Z_OK) ok = false;
    in.close();

    error_code ec;
    filesystem::remove(ok ? filesystem::path(source) : filesystem::path(target), ec);
    return ok;
}

FileSink::FileSink(const SinkConfig& config) {
    auto pathEntry = config.details.find("path");
    if (pathEntry == config.details.end() || pathEntry->second.empty()) {
        throw invalid_argument("provide a file path");
    }
    path = pathEntry->second;

    auto maxSizeEntry = config.details.find("maxFileSize");
    if (maxSizeEntry != config.details.end()) {
        const auto size = stoll(maxSizeEntry->second);
        if (size <=0) {
            throw invalid_argument("invalid maxSize");
        }
        maxFileSize = static_cast<uint64_t>(size);
    }

    openFile();
    if (!file) {
        throw runtime_error("can't get file" + path);
    }

    error_code ec;
    const auto size = filesystem::file_size(path, ec);
    currentSize = ec ? 0 : size;

    queueLeftoverRotations();
    compressor = thread(&FileSink::compressPending, this);
}

FileSink::~FileSink() {
    {
        lock_guard<mutex> lock(compressionMutex);
        stopping = true;
    }
    compressionReady.notify_one();
    if (compressor.joinable()) compressor.join();
}

void FileSink::openFile() {
    file.clear();
    file.open(path, ios::app | ios::binary);
}

void FileSink::queueLeftoverRotations() {
    const filesystem::path logPath(path);
    const auto dir = logPath.has_parent_path() ? logPath.parent_path() : filesystem::path(".");
    const auto prefix = logPath.filename().string() + ".";

    error_code ec;
    for (const auto& entry : filesystem::directory_iterator(dir, ec)) {
        const auto name = entry.path().filename().string();
        if (name.compare(0, prefix.size(), prefix) != 0) continue;
        if (isRotationTimestamp(name.substr(prefix.size()))) {
            pendingCompression.push(entry.path().string());
        }
    }
}

void FileSink::rotate() {
    const auto rotatedPath = path + "." + rotationTimestamp();

    error_code ec;
    if (filesystem::exists(rotatedPath, ec) || filesystem::exists(rotatedPath + ".gz", ec)) return;

    file.close();
    filesystem::rename(path, rotatedPath, ec);
    openFile();
    if (ec) return;

    currentSize = 0;
    {
        lock_guard<mutex> lock(compressionMutex);
        pendingCompression.push(rotatedPath);
    }
    compressionReady.notify_one();
}

void FileSink::compressPending() {
    unique_lock<mutex> lock(compressionMutex);
    while (true) {
        compressionReady.wait(lock, [this] { return stopping || !pendingCompression.empty(); });
        if (pendingCompression.empty()) return;

        const auto source = pendingCompression.front();
        pendingCompression.pop();
        lock.unlock();
        gzipFile(source);
        lock.lock();
    }
}

bool FileSink::write(const Message& message) {
    const string line = message.timestamp + " "
        + "[" + toString(message.level) + "] "
        + message.sourceNamespace + ": "
        + message.content + '\n';

    lock_guard<mutex> lock(fileMutex);
    if (!file.is_open()) {
        openFile();
    }
    if (maxFileSize > 0 && currentSize > 0 && currentSize + line.size() > maxFileSize) {
        rotate();
    }

    file.write(line.data(), static_cast<streamsize>(line.size()));
    file.flush();
    if (!file) return false;

    currentSize += line.size();
    return true;
}
