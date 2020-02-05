#pragma once

#include <arepa/networking/Socket.hpp>

#include <atomic>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <memory>

namespace arepa::networking::websocket {

/**
 * An implementation of Socket which uses the Beast websocket library.
 */
class BeastSocket : public arepa::networking::Socket, public std::enable_shared_from_this<BeastSocket> {

#pragma mark - Types -
public:
    typedef boost::beast::websocket::stream<boost::beast::tcp_stream> BeastSocketConnection;


#pragma mark - Fields -
private:
    std::atomic<bool> _connected = true;
    std::shared_ptr<BeastSocketConnection> _connection;
    boost::beast::flat_buffer _buffer;


#pragma mark - Methods (Protected) -
private:
    static NetworkException convert_error(boost::beast::error_code ec);


#pragma mark - Methods (Private) -
private:
    void _on_async_write(boost::beast::error_code ec, std::size_t transferred);
    void _on_async_read(boost::beast::error_code ec, std::size_t transferred);
    void _do_async_read();


#pragma mark - Constructors -
public:
    explicit BeastSocket(std::shared_ptr<BeastSocketConnection> connection) noexcept(false);
    ~BeastSocket() override = default;


#pragma mark - Methods -
public:
    void internal_start_reading();


#pragma mark - Override: Socket -
public:
    bool is_connected() override;
    void close(std::string reason) override;
    void close() override;
    void send(Data data) override;
};

}
