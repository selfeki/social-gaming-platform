#include "Formatter.hpp"

#include "arepa/log/Entry.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using namespace arepa::log::console;
using arepa::log::Entry;
using std::istringstream;
using std::setfill;
using std::setw;
using std::string;
using std::stringstream;
using std::tm;
using std::vector;
using std::chrono::system_clock;

string Formatter::format_entry(const Entry& entry) const {
    stringstream formatted;
    formatted << this->format_timestamp(entry.timestamp)
              << " " << this->format_module(entry.module)
              << " " << this->format_level(entry.level)
              << ": " << this->format_message(entry.message)
              << this->format_values(entry.values);

    return formatted.str();
}

string Formatter::format_module(const string& module) const {
    return module;
}

string Formatter::format_timestamp(const Entry::Timestamp& timestamp) const {
    // Convert the timestamp into something usable.
    tm time;    // NOLINT
    time_t timeUnix = system_clock::to_time_t(timestamp);
    gmtime_r(&timeUnix, &time);

    // Format.
    stringstream formatted;
    formatted << "["
              << std::setfill('0') << std::setw(2) << time.tm_hour << ':'
              << std::setfill('0') << std::setw(2) << time.tm_min << ':'
              << std::setfill('0') << std::setw(2) << time.tm_sec
              << "]";

    return formatted.str();
}

string Formatter::format_level(const Entry::Level& level) const {
    switch (level) {
    case Entry::Level::DEBUG:
        return "DEBUG";
    case Entry::Level::WARNING:
        return "WARN ";
    case Entry::Level::INFO:
        return "INFO ";
    case Entry::Level::FATAL:
        return "CRIT ";
    case Entry::Level::ERROR:
        return "ERROR";
    }
}

string Formatter::format_message(const string& message) const {
    return message;
}

string Formatter::format_values(const vector<string>& values) const {
    if (values.empty()) {
        return "";
    }

    stringstream formatted;
    size_t index = 0;
    for (const auto& value : values) {
        formatted << "\n"
                  << this->format_value(index++, value);
    }
    return formatted.str();
}

string Formatter::format_value(size_t index, const string& value) const {
    return "           [" + std::to_string(index) + "]> " + value;
}
