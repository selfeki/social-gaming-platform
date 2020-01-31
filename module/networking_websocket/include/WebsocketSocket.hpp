#pragma once

#include <arepa/networking/Socket.hpp>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace arepa::networking::websocket {

class WebsocketSocket : public arepa::networking::Socket {

#pragma mark - Fields -
private:
#pragma mark - Signals -
public:
    /**
     * A signal for when the data is received from the socket.
     */
    arepa::communication::Signal<const Data&> on_data;

    /**
     * A signal for when the socket is closed.
     */
    arepa::communication::Signal<arepa::communication::SocketException> on_close;

    /**
     * A signal for when the socket is closed due to an error.
     */
    arepa::communication::Signal<arepa::communication::SocketException> on_error;


#pragma mark - Methods (Private) -
private:
    void
    _on_async_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
    void _do_async_accept();


#pragma mark - Constructors -
public:
    Socket() noexcept(false);
    ~Socket();

#pragma mark - Methods -
public:
    /**
     * Closes the socket.
     * @param reason If applicable, the reason for closing the socket.
     */
    virtual void close(std::string reason) = 0;

    /**
     * Closes the socket.
     */
    virtual void close() = 0;

    /**
     * Sends data across the socket.
     * @param data The data to send.
     */
    virtual void send(Data data) = 0;
};

}
