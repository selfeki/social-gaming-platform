#pragma once

#include "Client.hpp"
#include "Connection.hpp"
#include "ConnectionId.hpp"

#include <arepa/communication/Signal.hpp>
#include <arepa/log/Log.hpp>
#include <arepa/networking/websocket/Listener.hpp>
#include <arepa/networking/websocket/Options.hpp>

#include <functional>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace arepa::server {

class Server : Log("Server") {
#pragma mark - Fields -
private:
    arepa::networking::websocket::Listener _listener;
    std::unordered_map<Connection::Id, Client> _clients;
    mutable std::shared_mutex _clients_lock;


#pragma mark - Signals -
public:
    /**
     * A signal called whenever a connection is accepted.
     */
    arepa::communication::Signal<std::shared_ptr<Connection>> on_accept;

    /**
     * A signal called whenever a connection is closed.
     */
    arepa::communication::Signal<std::shared_ptr<Connection>> on_close;


#pragma mark - Constructors -
public:
    Server(const arepa::networking::websocket::Options& options);


#pragma mark - Fields -
public:
    /**
     * Gets a vector of all the Client objects.
     * @return The client objects.
     */
    [[nodiscard]] std::vector<Client> clients() const;

    /**
     * Finds a Client by its connection ID.
     * 
     * @param id The connection ID.
     * @return The connection object.
     */
    [[nodiscard]] std::optional<Client> find_client(Connection::Id id) const;

    /**
     * Disconnects the Client specified by the given connection ID.
     * @pram id The client's connection ID.
     */
    void disconnect(Connection::Id id);

    /**
     * Disconnects the specified Client.
     * @param client The client to disconnect.
     */
    void disconnect(Client client);
};


}
