#include "Connection.hpp"

#include <arepa/networking/SessionToken.hpp>

using namespace arepa::server;
using arepa::communication::ChannelMultiQueue;
using arepa::communication::ChannelSingleQueue;
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
    this->_attach_signals();
}

Connection::Connection(Connection&& move) noexcept
    : _socket(std::move(move._socket))
    , _session(std::move(move._session)) {
    this->_attach_signals();
}

Connection::~Connection() {
    this->_socket.on_message.remove(this->_attach_socket_on_message);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Private) -
// ---------------------------------------------------------------------------------------------------------------------

void Connection::_attach_signals() {
    this->_attach_socket_on_message = this->_socket.on_message([this](const Message& message) {
        this->_channel.send(message);
    });
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

Connection::MessageQueue Connection::create_message_queue() {
    return this->_channel.create_consumer();
}

Connection::MessageQueue Connection::create_message_queue(ChannelMultiQueue<Message>::Filter filter) {
    return this->_channel.create_consumer(std::move(filter));
}

Session<>& Connection::session() {
    return this->_session;
}

const Session<>& Connection::session() const {
    return this->_session;
}

const SessionToken& Connection::session_token() const {
    return this->_session.token();
}

const SessionToken::Id& Connection::session_id() const {
    return this->_session.token().id();
}

MessageSocket& Connection::socket() {
    return this->_socket;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

Connection& Connection::operator<<(const arepa::protocol::Message& message) {
    this->_socket.send(message);
    return *this;
}

Connection& arepa::server::operator<<(std::shared_ptr<Connection>& connection, const arepa::protocol::Message& message) {
    return *connection << message;
}