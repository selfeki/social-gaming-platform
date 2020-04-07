#include "ServerUser.hpp"

#include <arepa/Util.hpp>

using namespace arepa::server;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

ServerUser::ServerUser(const std::shared_ptr<arepa::server::Connection>& connection)
    : arepa::chat::User(connection->id())
    , _connection(connection) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (UserConnection) -
// ---------------------------------------------------------------------------------------------------------------------

void ServerUser::send_message(const std::string_view& message) {
    this->_connection->send_message(message);
}

void ServerUser::send_packet(const UserCommunication::Packet& packet) {
    this->_connection->send_packet(packet);
}

void ServerUser::send_error_message(const std::string_view& message) {
    this->_connection->send_error_message(message);
}

void ServerUser::send_system_message(const std::string_view& message) {
    this->_connection->send_system_message(message);
}
