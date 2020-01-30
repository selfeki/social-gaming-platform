#include "FormatterWithColor.hpp"

#include "Formatter.hpp"
#include "arepa/log/Entry.hpp"

#include <chrono>
#include <string>

using namespace arepa::log::console;
using arepa::log::Entry;
using std::string;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Overrides: Formatter -
// ---------------------------------------------------------------------------------------------------------------------

string FormatterWithColor::format_module(const string& module) const {
    return "\x1B[35m" + Formatter::format_module(module) + "\x1B[0m";
}

string FormatterWithColor::format_timestamp(const Entry::Timestamp& timestamp) const {
    return "\x1B[37m" + Formatter::format_timestamp(timestamp) + "\x1B[0m";
}

string FormatterWithColor::format_level(const Entry::Level& level) const {
    switch (level) {
    case Entry::Level::DEBUG:
        return "\x1B[2;37mDEBUG\x1B[0m";
    case Entry::Level::WARNING:
        return "\x1B[33mWARN \x1B[0m";
    case Entry::Level::INFO:
        return "\x1B[37mINFO \x1B[0m";
    case Entry::Level::FATAL:
        return "\x1B[1;31mCRIT \x1B[0m";
    case Entry::Level::ERROR:
        return "\x1B[31mERROR\x1B[0m";
    }
}

string FormatterWithColor::format_message(const string& message) const {
    return message;
}

string FormatterWithColor::format_value(size_t index, const string& value) const {
    return "           \x1B[36m[" + std::to_string(index) + "]>\x1B[34m " + value + "\x1B[0m";
}
