#pragma once

#include <arepa/networking/Socket.hpp>

#include <atomic>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <memory>
#include <mutex>
#include <queue>

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
    std::atomic<bool> _write_finished = true;
    std::shared_ptr<BeastSocketConnection> _connection;
    boost::beast::flat_buffer _read_buffer;
    boost::beast::flat_buffer _write_buffer;
    std::queue<Data> _write_queue;
    std::mutex _write_lock;


#pragma mark - Methods (Protected) -
private:
    static NetworkException convert_error(boost::beast::error_code ec);


#pragma mark - Methods (Private) -
private:
    bool _handle_error(const boost::beast::error_code& ec);
    void _on_async_write(boost::beast::error_code ec, std::size_t transferred);
    void _on_async_read(boost::beast::error_code ec, std::size_t transferred);
    void _on_async_close(boost::beast::error_code ec);
    void _do_async_read();
    void _do_async_write();


#pragma mark - Constructors -
public:
    explicit BeastSocket(std::shared_ptr<BeastSocketConnection> connection);
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
