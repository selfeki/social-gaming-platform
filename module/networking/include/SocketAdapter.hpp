#pragma once

#include "Socket.hpp"

namespace arepa::networking {

/**
 * A wrapper around a Socket which serializes and deserializes structured data.
 */
template <typename Message>
class SocketAdapter {

#pragma mark - Types -
public:
    typedef std::vector<uint8_t> Data;


#pragma mark - Fields -
private:
    std::shared_ptr<Socket> _socket;
    decltype(_socket->on_data)::ListenerID _attach_socket_on_data;
    decltype(_socket->on_error)::ListenerID _attach_socket_on_error;
    decltype(_socket->on_close)::ListenerID _attach_socket_on_close;

#pragma mark - Signals -
public:
    /**
     * A signal for when a message is received from the socket.
     */
    arepa::communication::Signal<const Message&> on_message;

    /**
     * A signal for when the socket is closed.
     */
    arepa::communication::Signal<> on_close;

    /**
     * A signal for when the socket is closed due to an error.
     */
    arepa::communication::Signal<NetworkException> on_error;


#pragma mark - Constructors -
public:
    SocketAdapter(std::shared_ptr<Socket> socket)
        : _socket(std::move(socket)) {
        this->_attach_signals();
    }

    SocketAdapter(SocketAdapter&& move)
        : _socket(std::move(move._socket)) {
        this->_attach_signals();
    }

    SocketAdapter(const SocketAdapter& copy)
        : _socket(copy._socket) {
        this->_attach_signals();
    }

    ~SocketAdapter() {
        this->_socket->on_data.remove(this->_attach_socket_on_data);
        this->_socket->on_error.remove(this->_attach_socket_on_error);
        this->_socket->on_close.remove(this->_attach_socket_on_close);
    }


#pragma mark - Private -
private:
    void _attach_signals() {
        this->_attach_socket_on_error = this->_socket->on_error.forward(this->on_error);
        this->_attach_socket_on_close = this->_socket->on_close.forward(this->on_close);
        this->_attach_socket_on_data = this->_socket->on_data.listen([this](const Socket::Data& data) {
            auto message = Message::from_bytes(data);
            this->on_message.emit(message);
        });
    };


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
    /**
     * Closes the socket.
     * @param reason If applicable, the reason for closing the socket.
     */
    void close(std::string reason) {
        this->_socket->close(reason);
    }

    /**
     * Closes the socket.
     */
    void close() {
        this->_socket->close();
    }

    /**
     * Checks if the socket is still connected.
     * @return True if connected, false otherwise.
     */
    bool is_connected() {
        return this->_socket->is_connected();
    }
};

}
