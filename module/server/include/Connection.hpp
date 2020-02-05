#pragma once

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


#pragma mark - Constructors -
public:
    explicit Connection(std::shared_ptr<arepa::networking::Socket> socket);
    Connection(Connection&& move) noexcept;
    Connection(const Connection& copy) = delete;
    ~Connection();


#pragma mark - Methods -
public:
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
     * Returns the session token for the connection.
     * @return A reference to the session token.
     */
    [[nodiscard]] inline const arepa::networking::SessionToken& session_token() const;

    /**
     * Returns the session ID for the connection.
     * @return A reference to the session ID.
     */
    [[nodiscard]] inline const arepa::networking::SessionToken::Id& session_id() const;
};
}
