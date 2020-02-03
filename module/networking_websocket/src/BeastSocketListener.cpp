#include "BeastSocketListener.hpp"

#include "BeastContext.hpp"

#include <arepa/networking/NetworkException.hpp>

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <memory>

using namespace arepa::networking::websocket;
namespace beast = boost::beast;
namespace asio = boost::asio;
using std::make_unique;
using std::unique_ptr;
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

void BeastSocketListener::_on_async_accept(beast::error_code ec, boost_socket tcp_socket) {
    this->_do_async_accept();

    // Handle any potential error.
    if (ec) {
        //TODO(ethan): Error handling.
        return;
    }

    // Configure the socket.
    auto socket = make_unique<BeastWebsocket>(std::move(tcp_socket));
    socket->set_option(beast::websocket::stream_base::timeout::suggested(beast::role_type::server));
    socket->set_option(beast::websocket::stream_base::decorator([](beast::websocket::response_type& response) {
        response.set(beast::http::field::server, "arepa-ws");
    }));

    // Accept the socket as a websocket.
    socket->async_accept(
        beast::bind_front_handler(&BeastSocketListener::_on_async_accept_websocket, shared_from_this(), std::move(socket)));
}

#include <iostream>
void BeastSocketListener::_on_async_accept_websocket(unique_ptr<BeastWebsocket> socket, boost::beast::error_code ec) {
    if (ec) {
        //TODO(ethan): Error handling.
        return;
    }

    // TODO: Move the socket into a WebsocketSocket object.
}
