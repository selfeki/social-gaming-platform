#pragma once

#include <arepa/collections/Enum.hpp>

namespace arepa::log {

/**
 * An enum representing the severity of a log event.
 */
class Level {
public:
    enum Value {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    /**
     * Gets the string value of the log level.
     * @return The log level string.
     */
    [[nodiscard]] constexpr const char* to_string() {
        switch (static_cast<Value>(*this)) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case FATAL:
            return "FATAL";
        }
    }

    Enum(Level);
};
}
