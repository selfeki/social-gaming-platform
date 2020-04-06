#pragma once

#include "Connection.hpp"

#include <arepa/chat/User.hpp>

namespace arepa::server {

/**
 * A class that extends AbstractUser while implementing its communication interface.
 * This is the primary class for game users.
 */
class ServerUser : public arepa::chat::User {
#pragma mark - Fields -
private:
    std::shared_ptr<arepa::server::Connection> _connection;


#pragma mark - Constructors -
public:
    /**
     * Creates a new game user for a network connection.
     * @param connection The network connection.
     */
    explicit ServerUser(const std::shared_ptr<arepa::server::Connection>& connection);


#pragma mark - Methods (UserConnection) -
public:
    void send_message(const std::string_view& message) override;
    void send_error_message(const std::string_view& message) override;
    void send_system_message(const std::string_view& message) override;
    void send_packet(const Packet& packet) override;
};

}
