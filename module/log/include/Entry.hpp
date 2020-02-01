#pragma once

#include "Collector.hpp"
#include "Level.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace arepa::log {

/**
 * A log entry.
 * This struct contains information for a log event.
 */
class Entry {

#pragma mark - Types -
public:
    typedef std::chrono::system_clock::time_point Timestamp;
    typedef arepa::log::Level Level;


#pragma mark - Fields -
public:
    /**
     * The module that created the log event.
     */
    const std::string module;

    /**
     * The severity level of the log event.
     */
    const Level level;

    /**
     * The timestamp of the log event.
     */
    const Timestamp timestamp;

    /**
     * The event message.
     */
    const std::string message;

    /**
     * The additional values given to the event.
     */
    const std::vector<std::string> values;


#pragma mark - Constructors -
public:
    /**
     * Creates a new log event with additional context information.
     * 
     * @param module The module.
     * @param level The severity.
     * @param timestamp The timestamp.
     * @param message The message.
     * @param values Additional values which add context or debug information to the log entry.
     *
     * @tparam Collector The stream-like type to collect the extra values into a string vector.
     * @tparam As Template parameter for vararg parameters.
     */
    template <typename Collector = arepa::log::Collector, typename... As>
    Entry(std::string module, Level level, Timestamp timestamp, std::string message, const As&... values)
        : module(std::move(module))
        , level(level)
        , timestamp(timestamp)
        , message(std::move(message))
        , values(Collector::collect(values...)) {}

    /**
     * Creates a new log event with additional context information.
     * This will set the timestamp to the current time.
     *
     * @param module The module.
     * @param level The severity.
     * @param message The message.
     * @param values Additional values which add context or debug information to the log entry.
     *
     * @tparam Collector A class that implements a static Collect(As...) -> vector<string> interface.
     */
    template <typename Collector = arepa::log::Collector, typename... As>
    Entry(std::string module, Level level, std::string message, const As&... values)
        : module(std::move(module))
        , level(level)
        , timestamp(std::chrono::system_clock::now())
        , message(std::move(message))
        , values(Collector::collect(values...)) {}
};

}
