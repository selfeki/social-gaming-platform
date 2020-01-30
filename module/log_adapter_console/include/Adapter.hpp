#pragma once

#include "Formatter.hpp"

#include "arepa/log/Adapter.hpp"
#include "arepa/log/Collector.hpp"
#include "arepa/log/Entry.hpp"

#include <memory>
#include <mutex>
#include <string>

namespace arepa::log::console {

/**
 * An implementation of a log adapter that prints straight to the user's console.
 */
class Adapter : public arepa::log::Adapter {
private:
    std::mutex _mutex;
    std::unique_ptr<Formatter> _formatter;

public:
    Adapter();

    /**
     * Change the formatter that will be used to format log events.
     * @param formatter The new formatter.
     */
    void use_formatter(std::unique_ptr<Formatter> formatter);

    // Overrides:
    void process_log(const arepa::log::Entry& entry) override;
};

}
