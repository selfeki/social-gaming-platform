#pragma once

#include "Adapter.hpp"
#include "Log.hpp"
#include "LogStream.hpp"

#include <memory>
#include <optional>
#include <string>

namespace arepa::log {

/**
 * A factory that creates a new LogStream object every time something tries to write to it.
 * This is useful for replacing std::cout with a LogStream.
 */
class LogStreamFactory {

#pragma mark - Fields -
private:
    std::optional<std::shared_ptr<Adapter>> _adapter;
    std::optional<std::string> _module;

#pragma mark - Constructors -
public:
    explicit LogStreamFactory() noexcept;
    explicit LogStreamFactory(std::string module) noexcept;
    explicit LogStreamFactory(std::shared_ptr<Adapter> adapter, std::string module);
    explicit LogStreamFactory(std::shared_ptr<Adapter> adapter);
    ~LogStreamFactory() = default;

#pragma mark - Operators -
public:
    /**
     * Adds extra data to the log entry.
     * @param data The extra data.
     */
    template <typename T>
    LogStream operator<<(T data) const {
        std::shared_ptr<Adapter> adapter = this->_adapter ? *this->_adapter : global;

        if (this->_module) {
            LogStream stream(adapter, *this->_module);
            stream << data;
            return stream;
        } else {
            LogStream stream(adapter);
            stream << data;
            return stream;
        }
    }
};

}
