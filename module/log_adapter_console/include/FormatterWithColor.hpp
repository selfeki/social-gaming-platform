#pragma once

#include "Formatter.hpp"

#include <string>
#include <vector>

namespace arepa::log::console {

/**
 * A class that formats log entries to human-readable text.
 * This uses ANSI escape-sequence colors to make it more readable.
 */
class FormatterWithColor : public Formatter {

#pragma mark - Overrides: Formatter -
public:
    [[nodiscard]] std::string format_module(const std::string& module) const override;
    [[nodiscard]] std::string format_timestamp(const arepa::log::Entry::Timestamp& timestamp) const override;
    [[nodiscard]] std::string format_level(const arepa::log::Entry::Level& level) const override;
    [[nodiscard]] std::string format_message(const std::string& message) const override;
    [[nodiscard]] std::string format_value(size_t index, const std::string& value) const override;
};

}
