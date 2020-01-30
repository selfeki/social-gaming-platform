#pragma once

#include "Collector.hpp"
#include "Entry.hpp"
#include "Level.hpp"

#include <chrono>
#include <string>

#include <iostream>

namespace arepa::log {

/**
 * A class which provides an interface to store or print log entries.
 * @tparam Collector The class used to collect extra values into a vector.
 */
class Adapter {
protected:
    /**
     * Virtual function to process a log entry.
     * This can be stored, sent to the console, or completely ignored.
     *
     * @param entry The log entry to process.
     */
    virtual void process_log(const Entry& entry) = 0;

    /**
     * Template function to collect values into a string vector.
     *
     * @param values The values.
     * @return The collected values.
     */
    template <typename... As>
    std::vector<std::string> collect_values(As... values) const {
        Collector valuesCollector;
        (valuesCollector << ... << values);

        return valuesCollector.vec();
    }

public:
    virtual ~Adapter() = default;

    /**
     * Creates a log entry.
     *
     * @param module The module.
     * @param level The log level.
     * @param message The log message.
     * @param values Additional values.
     *
     * @return A log entry created from the provided data.
     */
    template <typename... As>
    [[nodiscard]] Entry entry(const std::string& module, const Entry::Level& level, const std::string& message, const As&... values) const {
        return this->entry(module, level, std::chrono::system_clock::now(), message, values...);
    }

    /**
     * Creates a log entry.
     *
     * @param module The module.
     * @param level The log level.
     * @param timestamp The timestamp.
     * @param message The log message.
     * @param values Additional values.
     *
     * @return A log entry created from the provided data.
     */
    template <typename... As>
    [[nodiscard]] Entry entry(const std::string& module, const Entry::Level& level, const Entry::Timestamp& timestamp, const std::string& message, const As&... values) const {
        return Entry(module, level, timestamp, message, std::move(this->collect_values(values...)));
    }

    /**
     * Logs an entry.
     * Use the entry function to create an entry object.
     * 
     * @param entry The log entry.
     */
    void log(const Entry& entry) {
        this->process_log(entry);
    }
};

}
