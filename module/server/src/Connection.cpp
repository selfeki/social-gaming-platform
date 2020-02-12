#include "Connection.hpp"

#include <arepa/networking/SessionToken.hpp>

#include <string_view>

using namespace arepa::server;
using arepa::communication::ChannelMultiQueue;
using arepa::communication::ChannelSingleQueue;
using arepa::networking::Session;
using arepa::networking::SessionToken;
using arepa::networking::Socket;
using arepa::protocol::Message;
using std::shared_ptr;
using std::string;
using std::string_view;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Connection::Connection(shared_ptr<Socket> socket)
    : _socket(ConnectionSocket(std::move(socket)))
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
    this->_attach_socket_on_message = this->_socket.on_message([this](const UnstructuredPacket& packet) {
        this->_channel.send(packet);
    });
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

Connection::PacketQueue Connection::create_packet_queue() {
    return this->_channel.create_consumer();
}

Connection::PacketQueue Connection::create_packet_queue(ChannelMultiQueue<UnstructuredPacket>::Filter filter) {
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

ConnectionSocket& Connection::socket() {
    return this->_socket;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

Connection& Connection::operator<<(const UnstructuredPacket& packet) {
    this->_socket.send(packet);
    return *this;
}

Connection& Connection::operator<<(const string_view& text) {
    return *this << UnstructuredPacket(string(text));
}

Connection& Connection::operator<<(const string& text) {
    return *this << string_view(text);
}

Connection& Connection::operator<<(const char* text) {
    return *this << string_view(text);
}
