#pragma once

#include "Formatter.hpp"

#include <arepa/log/Adapter.hpp>
#include <arepa/log/Collector.hpp>
#include <arepa/log/Entry.hpp>

#include <memory>
#include <mutex>
#include <string>

namespace arepa::log::console {

/**
 * An implementation of a log adapter that prints to the console's standard out.
 */
class ConsoleAdapter : public arepa::log::Adapter {

#pragma mark - Fields -
private:
    std::mutex _mutex;
    std::unique_ptr<Formatter> _formatter;


#pragma mark - Constructors -
public:
    ConsoleAdapter();


#pragma mark - Methods -
public:
    /**
     * Change the log formatter.
     * The log formatters are used to convert log events into human-friendly text.
     * @param formatter The new formatter to use.
     */
    void use_formatter(std::unique_ptr<Formatter> formatter);


#pragma mark - Overrides: Adapter -
public:
    void log(const arepa::log::Entry& entry) override;
};

}
