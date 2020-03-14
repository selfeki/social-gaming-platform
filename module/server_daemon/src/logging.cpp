#include "./logging.hpp"

arepa::log::LogStreamFactory clout;

void init_logging() {
    auto adapter = std::make_shared<arepa::log::console::ConsoleAdapter>();
    adapter->use_formatter(std::make_unique<arepa::log::console::FormatterWithColor>());
    arepa::log::global = adapter;
}
