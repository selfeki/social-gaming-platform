#pragma once

#include "Level.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace arepa::log {

/**
 * An unprocessed entry in the log.
 */
struct Entry {
    typedef std::chrono::system_clock::time_point Timestamp;
    typedef arepa::log::Level Level;

    const std::string module;
    const Level level;
    const Timestamp timestamp;
    const std::string message;
    const std::vector<std::string> values;

    Entry(std::string module, Level level, Timestamp timestamp, std::string message, std::vector<std::string> values);
};

}
