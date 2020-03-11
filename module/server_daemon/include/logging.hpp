#pragma once

#include <arepa/log/Level.hpp>
#include <arepa/log/LogStream.hpp>
#include <arepa/log/LogStreamFactory.hpp>
#include <arepa/log/console/ConsoleAdapter.hpp>
#include <arepa/log/console/FormatterWithColor.hpp>

constexpr arepa::log::Level ERROR = arepa::log::Level::ERROR;
constexpr arepa::log::Level WARN = arepa::log::Level::WARNING;

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
