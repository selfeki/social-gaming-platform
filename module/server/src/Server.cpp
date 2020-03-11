#include "Server.hpp"

#include <arepa/Util.hpp>
#include <arepa/networking/Options.hpp>
#include <arepa/networking/websocket/Listener.hpp>

using namespace arepa::server;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Server::Server(const arepa::networking::websocket::Options& options)
    : _listener(options) {

    this->_listener.start();
    this->_listener.on_accept([this](std::shared_ptr<arepa::networking::Socket> raw_socket) {
        auto connection = std::make_shared<Connection>(raw_socket);
        auto ptr = std::weak_ptr<Connection>(connection);

        // Client socket close handler.
        connection->socket().on_close([this, ptr]() {
            auto connection = ptr.lock();

            if (this->find_client(connection->id())) {
                log::info("Client connection closed.", connection->id());
                this->disconnect(connection->id());
            }
        });

        // Add client to connection list.
        {
            std::unique_lock guard(this->_clients_lock);
            this->_clients.emplace(connection->id(), Client(connection));
        }

        // Call the callback...
        log::info("Client connection accepted.", connection->id());
        this->on_accept.emit(connection);
    });
}

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

std::vector<Client> Server::clients() const {
    std::shared_lock guard(this->_clients_lock);

    // Create a copy.
    // This prevents concurrency issues.
    std::vector<Client> clients;
    for (const auto& pair : this->_clients) {
        clients.push_back(pair.second);
    }

    // Return the copy.
    return clients;
}

std::optional<Client> Server::find_client(Connection::Id id) const {
    std::shared_lock guard(this->_clients_lock);
    return arepa::find_in_map(this->_clients, id);
}

void Server::disconnect(Client client) {
    this->disconnect(client.connection->id());
}

void Server::disconnect(Connection::Id id) {
    std::unique_lock guard(this->_clients_lock);
    auto find = this->_clients.find(id);
    if (find == this->_clients.end()) {
        log::warning("Tried to disconnect a client that was not in the connection map.", id);
        return;
    }

    log::info("Client disconnected.", id);
    this->on_close.emit(find->second.connection);
    this->_clients.erase(find);
}
