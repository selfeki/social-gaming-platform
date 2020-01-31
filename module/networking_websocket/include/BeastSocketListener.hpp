#pragma once

#include "BeastContext.hpp"

#include <arepa/communication/Signal.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>

namespace arepa::networking::websocket {

/**
 * A class that binds a TCP endpoint and accepts websockets.
 * This uses boost::asio and boost::beast to handle the underlying I/O opertaions.
 */
class BeastSocketListener : public std::enable_shared_from_this<BeastSocketListener> {

#pragma mark - Fields -
private:
    BeastContext& _context;
    boost::asio::ip::tcp::acceptor _acceptor;


#pragma mark - Signals -
public:
    //    /**
    //     * A signal for when a new socket connection failed to be established.
    //     */
    //    arepa::communication::Signal<arepa::communication::SocketException> on_error;

    //    /**
    //     * A signal for when a new socket connection is established.
    //     */
    //    arepa::communication::Signal<arepa::communication::Socket> on_accept;


#pragma mark - Methods (Private) -
private:
    void _on_async_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
    void _do_async_accept();


#pragma mark - Constructors -
public:
    explicit BeastSocketListener(BeastContext& context) noexcept(false);

#pragma mark - Methods -
public:
    /**
     * Starts listening for socket connections.
     */
    void start() noexcept(false);

    /**
     * Stops listening for socket connections.
     */
    void stop() noexcept(false);
};

}
