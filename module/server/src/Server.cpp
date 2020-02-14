/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////


#include "Server.h"

#include <arepa/networking/Options.hpp>
#include <arepa/networking/websocket/Listener.hpp>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

using namespace std::string_literals;
using networking::Message;
using networking::Server;
using networking::ServerImpl;

using arepa::networking::websocket::Listener;
using arepa::networking::websocket::Options;

namespace networking {

Message::Message(ConnectionId id, string text)
    : connection(id)
    , text(text) {}

std::string ConnectionId::name() const {
    auto str = boost::uuids::to_string(this->uuid);
    return str.substr(0, 5);    // Let's not use giant UUIDs for names.
}

bool ConnectionId::operator==(const ConnectionId& rhs) const {
    return this->uuid == rhs.uuid;
}

bool ConnectionId::operator!=(const ConnectionId& rhs) const {
    return !(*this == rhs);
}

bool ConnectionId::operator==(const std::string& rhs) const {
    return this->name() == rhs || boost::uuids::to_string(this->uuid) == rhs;
}

bool ConnectionId::operator!=(const std::string& rhs) const {
    return !(*this == rhs);
}

ConnectionId::operator std::string() const {
    return this->name();
}

/////////////////////////////////////////////////////////////////////////////
// Core Server
/////////////////////////////////////////////////////////////////////////////

std::deque<Message> Server::receive() {
    std::deque<Message> messages;

    for (const auto& client : this->clients()) {
        while (const auto& message = client.messages->receive()) {
            messages.emplace_back(client.connection->session_id(), *message);
        }
    }

    return messages;
}


void Server::send(const std::deque<Message>& messages) {
    // FIXME(anyone?): This should probably be using a sorted set so you don't need to find the connection every time.

    for (auto& message : messages) {
        auto connection = this->connection(message.connection);
        if (connection) {
            *connection << message.text << "\n";
        }
    }
}


void Server::disconnect(ConnectionId id) {
    auto connection = this->connection(id);
    if (connection) {
        (*connection)->socket().close();
    }
}

std::vector<Server::Client> Server::clients() {
    std::shared_lock guard(this->_connections_lock);
    return this->_connections;
}

std::vector<std::shared_ptr<Connection>> Server::connections() {
    std::shared_lock guard(this->_connections_lock);
    std::vector<std::shared_ptr<Connection>> connections;

    for (const auto& client : this->_connections) {
        connections.push_back(client.connection);
    }

    return connections;
}

std::optional<std::shared_ptr<Connection>> Server::connection(ConnectionId id) {
    std::shared_lock guard(this->_connections_lock);
    auto find = std::find_if(this->_connections.begin(), this->_connections.end(), [id](const Server::Client& client) {
        return client.connection->session_id() == id.uuid;
    });

    if (find == this->_connections.end()) {
        return std::nullopt;
    }

    return { find->connection };
}


}