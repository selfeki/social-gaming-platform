#include "BeastSocket.hpp"

#include <boost/system/system_error.hpp>
#include <memory>

using namespace arepa::networking::websocket;
namespace beast = boost::beast;
namespace asio = boost::asio;
using boost::beast::error_code;
using boost::beast::websocket::close_reason;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::uint8_t;
using std::unique_ptr;
using BeastSocketConnection = BeastSocket::BeastSocketConnection;

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
#pragma mark - Methods (Protected) -
// ---------------------------------------------------------------------------------------------------------------------

arepa::networking::NetworkException BeastSocket::convert_error(boost::beast::error_code ec) {
    if (ec == boost::system::errc::connection_reset) {
        return NetworkException(NetworkException::REMOTE_CONNECTION_CLOSED);
    }

    return NetworkException(NetworkException::REMOTE_CONNECTION_ERROR);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Private) -
// ---------------------------------------------------------------------------------------------------------------------

void BeastSocket::_do_async_read() {
    this->_connection->async_read(this->_buffer,
        beast::bind_front_handler(&BeastSocket::_on_async_read,
            shared_from_this()));
}

void BeastSocket::_on_async_read(error_code ec, std::size_t transferred) {
    if (this->_handle_error(ec)) {
        return;
    }

    // Covert to data.
    auto buffer = static_cast<const uint8_t*>(this->_buffer.data().data());
    auto data = Data();
    data.reserve(transferred);
    std::copy(buffer, buffer + transferred, std::back_inserter(data));
    this->_buffer.clear();

    // Read next packet.
    this->_do_async_read();

    // Emit the data.
    this->on_data.emit(data);
}

void BeastSocket::_on_async_write(shared_ptr<Data>, error_code ec, std::size_t transferred) {
    if (this->_handle_error(ec)) {
        return;
    }
}

void BeastSocket::_on_async_close(error_code ec) {
    if (this->_handle_error(ec)) {
        return;
    }

    this->on_close();
}

bool BeastSocket::_handle_error(const error_code& ec) {
    if (!ec) {
        return false;
    }

    this->_connected = false;

    // If it's not an end_of_stream (remote close), it's an error.
    // Send an on_error signal.
    if (ec != boost::beast::http::error::end_of_stream) {
        this->on_error(BeastSocket::convert_error(ec));
    }

    // Send an on_close signal.
    this->on_close();
    return true;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Override: Socket -
// ---------------------------------------------------------------------------------------------------------------------

void BeastSocket::send(Data data) {
    // NOTE(ethan): We create a pointer and bind it to the callback so the buffer's underlying memory isn't freed until
    //              the callback has fired.
    auto buffer = make_shared<Data>(data);
    this->_connection->async_write(asio::buffer(*buffer), beast::bind_front_handler(&BeastSocket::_on_async_write, shared_from_this(), buffer));
}

void BeastSocket::close() {
    if (this->_connected.exchange(false)) {
        this->on_data.remove_all();

        this->_connection->async_close(close_reason(), beast::bind_front_handler(&BeastSocket::_on_async_close, shared_from_this()));
        this->_connection = nullptr;
    }
}

void BeastSocket::close(std::string reason) {
    if (this->_connected.exchange(false)) {
        this->on_data.remove_all();

        this->_connection->async_close(close_reason(reason), beast::bind_front_handler(&BeastSocket::_on_async_close, shared_from_this()));
        this->_connection = nullptr;
    }
}

bool BeastSocket::is_connected() {
    return this->_connected;
}
