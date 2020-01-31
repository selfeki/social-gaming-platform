#pragma once

#include <arepa/collections/Enum.hpp>

namespace arepa::log {

/**
 * An enum representing the severity of a log event.
 */
class Level {

#pragma mark - Enum -
public:
    enum Value {

        /**
         * A debug message.
         * These are used for debugging.
         */
        DEBUG,

        /**
         * An information message.
         */
        INFO,

        /**
         * A warning message.
         * These could indicate that something has went wrong, but was handled properly.
         */
        WARNING,

        /**
         * An error message.
         * These indicate that something went wrong, but it's possible to recover from.
         */
        ERROR,

        /**
         * A fatal error message.
         * This indicates that something went horribly wrong, and the entire module is likely to fail.
         */
        FATAL
    };


#pragma mark - Methods -
public:
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

#pragma mark - Macro: Enum -
    Enum(Level);
};
}
