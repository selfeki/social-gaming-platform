#include "BeastSocket.hpp"

#include <memory>

using namespace arepa::networking::websocket;
namespace beast = boost::beast;
namespace asio = boost::asio;
using boost::beast::websocket::close_reason;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::uint8_t;
using std::unique_ptr;
using BeastSocketConnection = BeastSocket::BeastSocketConnection;

#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

BeastSocket::BeastSocket(shared_ptr<BeastSocketConnection> connection) noexcept(false)
    : _connection(std::move(connection)) {
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

void BeastSocket::internal_start_reading() {
    this->_do_async_read();
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Private) -
// ---------------------------------------------------------------------------------------------------------------------

void BeastSocket::_do_async_read() {
    this->_connection->async_read(this->_buffer,
        beast::bind_front_handler(&BeastSocket::_on_async_read,
            shared_from_this()));
}

void BeastSocket::_on_async_read(boost::beast::error_code ec, std::size_t transferred) {
    if (ec) {
        //TODO(ethan): Error handling.
        return;
    }

    // Covert to data.
    auto buffer = static_cast<const uint8_t*>(this->_buffer.data().data());
    auto data = make_shared<Data>();
    data->reserve(transferred);
    std::copy(buffer, buffer + transferred, std::back_inserter(*data));

    // Read next packet.
    this->_do_async_read();

    // DEBUG
    this->send(*data);
    std::cout << "Got something!" << std::endl;
}

void BeastSocket::_on_async_write(boost::beast::error_code ec,
    std::size_t transferred) {
    if (ec) {
        //TODO(ethan): Error handling.
        return;
    }

    std::cout << "Sent something!" << std::endl;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Override: Socket -
// ---------------------------------------------------------------------------------------------------------------------

void BeastSocket::send(Data data) {
    this->_connection->async_write(asio::buffer(data), beast::bind_front_handler(&BeastSocket::_on_async_write, shared_from_this()));
}

void BeastSocket::close() {
    this->_connection->close(close_reason());
}

void BeastSocket::close(std::string reason) {
    this->_connection->close(close_reason(reason));
}
