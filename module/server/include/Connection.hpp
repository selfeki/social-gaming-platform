#pragma once

#include "ConnectionId.hpp"
#include "ConnectionSocket.hpp"

#include <arepa/chat/UserCommunication.hpp>
#include <arepa/communication/ChannelMultiQueue.hpp>
#include <arepa/communication/ChannelSingleQueue.hpp>
#include <arepa/networking/Session.hpp>
#include <arepa/networking/Socket.hpp>
#include <arepa/protocol/Message.hpp>
#include <arepa/protocol/Packet.hpp>

#include <memory>
#include <string_view>

namespace arepa::server {

/**
 * A client connection.
 */
class Connection : public arepa::chat::UserCommunication {
#pragma mark - Types -
public:
    using PacketQueue = std::shared_ptr<arepa::communication::ChannelSingleQueue<arepa::protocol::Packet>>;
    using Id = ConnectionId;


#pragma mark - Fields -
private:
    /**
     * The network socket for the connection.
     */
    ConnectionSocket _socket;

    /**
     * The client session information for the connection.
     */
    arepa::networking::Session<> _session;

    /**
     * The MPMC message channel.
     */
    arepa::communication::ChannelMultiQueue<arepa::protocol::Packet> _channel;

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
     * Creates a new packet queue for this connection.
     * All received packet will be copied to this queue, which can be consumed in a thread-safe manner.
     *
     * @return The new packet queue.
     */
    [[nodiscard]] PacketQueue create_packet_queue();

    /**
     * Creates a new filtered packet queue for this connection.
     * Packets will be copied to this queue, which can be consumed in a thread-safe manner.
     *
     * @param filter The filter function.
     *
     * @return The new packet queue.
     */
    [[nodiscard]] PacketQueue create_packet_queue(decltype(_channel)::Filter filter);

    /**
     * Returns the network socket for the connection.
     * @return A reference to the network socket.
     */
    [[nodiscard]] ConnectionSocket& socket();

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

    /**
     * Returns the connection ID.
     * @return A reference to the connection ID.
     */
    [[nodiscard]] const Id& id() const;

    void send_message(const char* message);


#pragma mark - Methods (PlayerNetworking) -
public:
    void send_message(const std::string_view& message) override;
    void send_error_message(const std::string_view& message) override;
    void send_system_message(const std::string_view& message) override;
    void send_packet(const arepa::protocol::Packet& packet) override;


#pragma mark - Operators -
public:
    // TODO(ethan): Remove the below operators in favor of just using send_* methods.
    Connection& operator<<(const arepa::protocol::Packet& message);
    Connection& operator<<(const std::string& message);
    Connection& operator<<(const std::string_view& message);
    Connection& operator<<(const char* message);
};

// TODO(ethan): Remove the operator template below.
template <typename T>
Connection& operator<<(const std::shared_ptr<Connection>& connection, const T& data) {
    return *connection << data;
}

}
