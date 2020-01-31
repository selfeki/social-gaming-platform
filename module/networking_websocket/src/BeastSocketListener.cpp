#include "BeastSocketListener.hpp"

#include "BeastContext.hpp"

#include <arepa/networking/NetworkException.hpp>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <iostream>

using namespace arepa::networking::websocket;
namespace beast = boost::beast;
namespace asio = boost::asio;
using boost_socket = boost::asio::ip::tcp::socket;
using boost_socket_base = boost::asio::socket_base;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

BeastSocketListener::BeastSocketListener(BeastContext& context)
    : _context(context)
    , _acceptor(*context.io_context) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

void BeastSocketListener::start() {
    try {
        this->_acceptor.open(this->_context.endpoint.protocol());
        this->_acceptor.set_option(boost_socket_base::reuse_address(true));
        this->_acceptor.bind(this->_context.endpoint);                        // Bind the endpoint.
        this->_acceptor.listen(boost_socket_base::max_listen_connections);    // Start listening.
    } catch (boost::system::system_error& error) {
        throw NetworkException(NetworkException::BIND_ERROR);
    }

    this->_do_async_accept();
}

void BeastSocketListener::stop() {
    this->_acceptor.close();
    // TODO(ethan): Find a way to propagate close to threads.
}

void BeastSocketListener::_do_async_accept() {
    this->_acceptor.async_accept(
        asio::make_strand(*this->_context.io_context),
        beast::bind_front_handler(&BeastSocketListener::_on_async_accept, shared_from_this()));
}

void BeastSocketListener::_on_async_accept(beast::error_code ec, boost_socket socket) {
    this->_do_async_accept();

    // TODO(ethan): Create a websocket from the TCP socket.
    // TODO(ethan): Fire a callback depending on success/failure.

    std::cout << "Hi" << std::endl;
}
