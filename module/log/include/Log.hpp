#pragma once

#include "Adapter.hpp"
#include "Entry.hpp"

#include <boost/metaparse.hpp>
#include <memory>
#include <sstream>

namespace arepa::log {
static std::shared_ptr<Adapter> global;

template <const char* Module>
class _Log {    // NOLINT
protected:
    /**
     * A struct enabling logging from a global context.
     */
    struct log {    // NOLINT
        /**
         * Logs an INFO-level message.
         * @param message The message.
         */
        template <typename... As>
        static void info(const std::string& message, const As&... values) {
            global->log(Entry(Module, Entry::Level::INFO, message, values...));
        }

        /**
         * Logs a DEBUG-level message.
         * @param message The message.
         */
        template <typename... As>
        static void debug(const std::string& message, const As&... values) {
            global->log(Entry(Module, Entry::Level::DEBUG, message, values...));
        }

        /**
         * Logs a WARNING-level message.
         * @param message The message.
         */
        template <typename... As>
        static void warning(const std::string& message, const As&... values) {
            global->log(Entry(Module, Entry::Level::WARNING, message, values...));
        }

        /**
         * Logs an ERROR-level message.
         * @param message The message.
         */
        template <typename... As>
        static void error(const std::string& message, const As&... values) {
            global->log(Entry(Module, Entry::Level::ERROR, message, values...));
        }

        /**
         * Logs a FATAL-level message.
         * @param message The message.
         */
        template <typename... As>
        static void fatal(const std::string& message, const As&... values) {
            global->log(Entry(Module, Entry::Level::FATAL, message, values...));
        }
    };
};

}

#pragma mark - Macros -

/**
 * A macro that, when used to privately extend a class, allows for global logging.
 */
#define Log(module) arepa::log::_Log<boost::mpl::c_str<BOOST_METAPARSE_STRING(module)>::value> /* NOLINT */
