#include "GameUser.hpp"

#include <arepa/Util.hpp>
#include <arepa/server/Connection.hpp>


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

GameUser::GameUser(const std::shared_ptr<arepa::server::Connection>& connection)
    : arepa::game::AbstractUser(connection->id())
    , _connection(connection) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (UserConnection) -
// ---------------------------------------------------------------------------------------------------------------------

void GameUser::send_message(const std::string_view& message) {
    this->_connection->send_message(message);
}

void GameUser::send_packet(const UserCommunication::Packet& packet) {
    this->_connection->send_packet(packet);
}

void GameUser::send_error_message(const std::string_view& message) {
    this->_connection->send_error_message(message);
}

void GameUser::send_system_message(const std::string_view& message) {
    this->_connection->send_system_message(message);
}
