#pragma once

#include <arepa/log/LogStream.hpp>
#include <arepa/log/LogStreamFactory.hpp>
#include <arepa/log/console/ConsoleAdapter.hpp>
#include <arepa/log/console/FormatterWithColor.hpp>

/**
 * Log extra data.
 */
using data = arepa::log::LogStream::data<>;

/**
 * Logging equivalent of `std::cout`.
 * Stands for: Console Log OUTput.
 */
extern arepa::log::LogStreamFactory clout;

/**
 * Logging equivalent of `std::endl`.
 */
constexpr auto endl = arepa::log::LogStream::endl;

/**
 * Initialize logging.
 */
void init_logging();
