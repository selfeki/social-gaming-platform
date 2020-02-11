#pragma once

#include "Options.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>

namespace arepa::networking::websocket {

/**
 * A context struct passed to underlying implementations that use Boost::Beast.
 * This contains and holds the io_context and threadpool, along with some extra configuration data.
 */
class BeastContext {

#pragma mark - Fields (Public) -
public:
    boost::asio::io_context* io_context;    // NOTE: Raw pointer needed because io_context should not be moved.
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_context_guard;
    std::vector<std::thread> threads;
    boost::asio::ip::tcp::endpoint endpoint;

#pragma mark - Constructors -
public:
    explicit BeastContext(const Options& options);
    BeastContext(const BeastContext& copy) = delete;
    ~BeastContext();
};

}
