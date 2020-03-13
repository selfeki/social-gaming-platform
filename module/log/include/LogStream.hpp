#pragma once

#include "Adapter.hpp"
#include "Collector.hpp"
#include "Entry.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace arepa::log {

/**
 * A stream class for writing logs in a std::cout-like fashion.
 */
class LogStream {
#pragma mark - Types -
public:
    /**
     * An extra value to add to the log entry.
     */
    template <typename Collector = arepa::log::Collector>
    class data {    //NOLINT
        friend LogStream;

    private:
        std::string _value;

    public:
        template <typename T>
        data(const T& value)
            : _value(Collector::collect(value).front()) {}
    };

#pragma mark - Manipulators -
public:
    enum _Manipulators {    //NOLINT

        /**
         * Flushes the log entry and resets the stream.
         */
        endl    // NOLINT
    };


#pragma mark - Fields -
private:
    std::shared_ptr<Adapter> _adapter;
    std::vector<std::string> _current_values;
    std::stringstream _current_text;
    std::string _current_module;
    Level _current_level;

#pragma mark - Constructors -
public:
    explicit LogStream(std::shared_ptr<Adapter> adapter, std::string module);
    explicit LogStream(std::shared_ptr<Adapter> adapter);
    ~LogStream();
    LogStream(const LogStream& copy);

#pragma mark - Methods (Private) -
private:
    /**
     * Reset the stream to a mostly-blank slate.
     * This will reset everything except the module name to default.
     */
    void _reset();

#pragma mark - Methods -
public:
    /**
     * Commit the log message created by this stream.
     * This will be automatically called when destructed.
     */
    void commit();

#pragma mark - Operators -
public:
    /**
     * Set the log level for this log stream instance.
     */
    LogStream& operator<<(Level level);

    /**
     * Appends to the log message.
     */
    LogStream& operator<<(const std::string& message);

    /**
     * Performs an action based on a manipulator.
     * @param manipulator The manipulator.
     * @return The stream.
     */
    LogStream& operator<<(_Manipulators manipulator);

    /**
     * Adds extra data to the log entry.
     * @param data The extra data.
     */
    template <typename Collector = arepa::log::Collector>
    LogStream& operator<<(const data<Collector>& data) {
        this->_current_values.push_back(data._value);
        return *this;
    }
};

}
