#include "Connection.hpp"

#include <arepa/networking/SessionToken.hpp>

using namespace arepa::server;
using arepa::networking::Session;
using arepa::networking::SessionToken;
using arepa::networking::Socket;
using arepa::protocol::Message;
using arepa::protocol::MessageSocket;
using std::shared_ptr;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Connection::Connection(shared_ptr<Socket> socket)
    : _socket(MessageSocket(std::move(socket)))
    , _session(SessionToken::generate()) {
}

Connection::Connection(Connection&& move) noexcept
    : _socket(std::move(move._socket))
    , _session(std::move(move._session)) {
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

Session<>& Connection::session() {
    return this->_session;
}

MessageSocket& Connection::socket() {
    return this->_socket;
}
