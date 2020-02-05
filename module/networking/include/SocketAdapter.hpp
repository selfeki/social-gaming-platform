#pragma once

#include "Socket.hpp"

namespace arepa::networking {

/**
 * A wrapper around a Socket which serializes and deserializes stuctured data.
 */
template <typename Message>
class SocketAdapter {

#pragma mark - Types -
public:
    typedef std::vector<uint8_t> Data;


#pragma mark - Fields -
private:
    std::shared_ptr<Socket> _socket;

#pragma mark - Signals -
public:
    /**
     * A signal for when a message is received from the socket.
     */
    arepa::communication::Signal<const Message&> on_message;

    /**
     * A signal for when the socket is closed.
     */
    arepa::communication::Signal<>& on_close;

    /**
     * A signal for when the socket is closed due to an error.
     */
    arepa::communication::Signal<NetworkException>& on_error;


#pragma mark - Constructors -
public:
    SocketAdapter(std::shared_ptr<Socket> socket)
        : _socket(std::move(socket))
        , on_close(this->_socket->on_close)
        , on_error(this->_socket->on_error) {
        this->_socket->on_data([this](const Socket::Data& data) {
            auto message = Message::from_bytes(data);
            this->on_message.emit(message);
        });
    }


#pragma mark - Methods -
public:
    /**
     * Sends a message across the socket.
     * This will call Message::to_bytes to convert the message type to a vector of bytes.
     * 
     * @param message The message to send.
     */
    template <typename T>
    void send(T&& message) {
        auto data = Message::to_bytes(std::forward<T>(message));
        this->_socket->send(data);
    }


#pragma mark - Override: Socket -
public:
    void close(std::string reason) {
        this->_socket->close(reason);
    }

    void close() {
        this->_socket->close();
    }
};

}
