#pragma once

#include <arepa/log/Entry.hpp>

#include <string>
#include <vector>

namespace arepa::log::console {

/**
 * A class that formats log entries to human-readable text.
 */
class Formatter {
#pragma mark - Constructors -
public:
    /**
    *
    */
    virtual ~Formatter() = default;


#pragma mark - Virtual -
public:
    /**
     * Formats a log entry into a string.
     *
     * @param entry The log entry.
     * @return The formatted string.
     */
    [[nodiscard]] virtual std::string format_entry(const arepa::log::Entry& entry) const;

    /**
     * Formats a log entry's module name.
     *
     * @param module The module name.
     * @return The formatted string.
     */
    [[nodiscard]] virtual std::string format_module(const std::string& module) const;

    /**
     * Formats a log entry's timestamp.
     *
     * @param timestamp The timestamp.
     * @return The formatted string.
     */
    [[nodiscard]] virtual std::string format_timestamp(const arepa::log::Entry::Timestamp& timestamp) const;

    /**
     * Formats a log entry's log level.
     *
     * @param level The log level.
     * @return The formatted string.
     */
    [[nodiscard]] virtual std::string format_level(const arepa::log::Entry::Level& level) const;

    /**
     * Formats a log entry's message.
     *
     * @param message The message.
     * @return The formatted string.
     */
    [[nodiscard]] virtual std::string format_message(const std::string& message) const;

    /**
     * Formats a log entry's values.
     *
     * @param values The values vector.
     * @return The formatted string.
     */
    [[nodiscard]] virtual std::string format_values(const std::vector<std::string>& values) const;

    /**
     * Formats a single value.
     * 
     * @param index The value index.
     * @param value The value string.
     * @return The formatted string.
     */
    [[nodiscard]] virtual std::string format_value(size_t index, const std::string& value) const;
};

}
