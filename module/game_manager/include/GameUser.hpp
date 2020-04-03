#pragma once

#include <arepa/game/AbstractUser.hpp>
#include <arepa/server/Connection.hpp>

//namespace arepa::game { // TODO(somebody): Move all the other files in this module into a namespace like this.

/**
 * A class that extends AbstractUser while implementing its communication interface.
 * This is the primary class for game users.
 */
class GameUser : public arepa::game::AbstractUser {
#pragma mark - Fields -
private:
    std::shared_ptr<arepa::server::Connection> _connection;


#pragma mark - Constructors -
public:
    /**
     * Creates a new game user for a network connection.
     * @param connection The network connection.
     */
    explicit GameUser(const std::shared_ptr<arepa::server::Connection>& connection);


#pragma mark - Methods (UserConnection) -
public:
    void send_message(const std::string_view& message) override;
    void send_error_message(const std::string_view& message) override;
    void send_system_message(const std::string_view& message) override;
    void send_packet(const Packet& packet) override;
};

// }
