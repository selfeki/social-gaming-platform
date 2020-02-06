#pragma once

#include <arepa/communication/ChannelMultiQueue.hpp>
#include <arepa/communication/ChannelSingleQueue.hpp>
#include <arepa/networking/Session.hpp>
#include <arepa/networking/Socket.hpp>
#include <arepa/protocol/Message.hpp>
#include <arepa/protocol/MessageSocket.hpp>

#include <memory>

namespace arepa::server {

/**
 * A client connection.
 */
class Connection {
#pragma mark - Types -
public:
    typedef std::shared_ptr<arepa::communication::ChannelSingleQueue<arepa::protocol::Message>> MessageQueue;


#pragma mark - Fields -
private:
    /**
     * The network socket for the connection.
     */
    arepa::protocol::MessageSocket _socket;

    /**
     * The client session information for the connection.
     */
    arepa::networking::Session<> _session;

    /**
     * The MPMC message channel.
     */
    arepa::communication::ChannelMultiQueue<arepa::protocol::Message> _channel;

    decltype(_socket.on_message)::ListenerID _attach_socket_on_message;


#pragma mark - Constructors -
public:
    explicit Connection(std::shared_ptr<arepa::networking::Socket> socket);
    Connection(Connection&& move) noexcept;
    Connection(const Connection& copy) = delete;
    ~Connection();


#pragma mark - Methods (Private) -
private:
    void _attach_signals();


#pragma mark - Methods -
public:
    /**
     * Creates a new message queue for this connection.
     * All received messages will be copied to this queue, which can be consumed in a thread-safe manner.
     *
     * @return The new message queue.
     */
    [[nodiscard]] MessageQueue create_message_queue();

    /**
     * Creates a new filtered message queue for this connection.
     * Messages will be copied to this queue, which can be consumed in a thread-safe manner.
     *
     * @param filter The filter function.
     *
     * @return The new message queue.
     */
    [[nodiscard]] MessageQueue create_message_queue(decltype(_channel)::Filter filter);

    /**
     * Returns the network socket for the connection.
     * @return A reference to the network socket.
     */
    [[nodiscard]] arepa::protocol::MessageSocket& socket();

    /**
     * Returns the session object for the connection.
     * @return A reference to the session object.
     */
    [[nodiscard]] arepa::networking::Session<>& session();

    /**
     * Returns the session object for the connection.
     * @return A reference to the session object.
     */
    [[nodiscard]] const arepa::networking::Session<>& session() const;

    /**
     * Returns the session token for the connection.
     * @return A reference to the session token.
     */
    [[nodiscard]] const arepa::networking::SessionToken& session_token() const;

    /**
     * Returns the session ID for the connection.
     * @return A reference to the session ID.
     */
    [[nodiscard]] const arepa::networking::SessionToken::Id& session_id() const;


#pragma mark - Operators -
public:
    Connection& operator<<(const arepa::protocol::Message& message);
};


Connection& operator<<(const std::shared_ptr<Connection>& connection, const arepa::protocol::Message& message);

}
