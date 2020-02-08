/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////


#ifndef NETWORKING_SERVER_H
#define NETWORKING_SERVER_H

#include "Connection.hpp"

#include <arepa/networking/SessionToken.hpp>
#include <arepa/networking/websocket/Listener.hpp>
#include <arepa/networking/websocket/Options.hpp>

#include <boost/uuid/uuid_hash.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <deque>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>

using arepa::server::Connection;
using std::make_shared;
using std::pair;
using std::shared_mutex;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;

namespace networking {

struct ConnectionId {
    arepa::networking::SessionToken::Id uuid;

    ConnectionId(arepa::networking::SessionToken::Id uuid)
        : uuid(uuid) {}

    std::string name() const;

    bool operator==(const ConnectionId& rhs) const;
    bool operator!=(const ConnectionId& rhs) const;
    bool operator==(const std::string& rhs) const;
    bool operator!=(const std::string& rhs) const;
    operator std::string() const;
};


/**
 *  A Message containing text that can be sent to or was recieved from a given
 *  Connection.
 */
struct Message {
    ConnectionId connection;
    std::string text;

    /**
     * Creates a new Message object.
     *
     * @param connection The connection ID.
     * @param text The message text.
     */
    Message(ConnectionId connection, std::string text);
};


class ServerImpl;


/**
 *  @class Server
 *
 *  @brief A single threaded network server for transferring text.
 *
 *  The Server class transfers text to and from multiple Client instances
 *  connected on a given port. The behavior is single threaded, so all transfer
 *  operations are grouped and performed on the next call to Server::update().
 *  Text can be sent to the Server using Client::send() and received from the
 *  Server using Client::receive().
 *
 *  The Server is websocket based and supports sending a single file back in
 *  response to HTTP requests for `index.html`. This allows command line and
 *  web clients to interact.
 */
class Server {
private:
    /**
     * A container struct that holds a client connection, and a client packet queue.
     */
    struct Client {
        shared_ptr<Connection> connection;
        Connection::PacketQueue messages;    // NOTE: This is a pointer.
    };

    arepa::networking::websocket::Listener _listener;
    vector<Client> _connections;
    shared_mutex _connections_lock;

protected:
    /**
     * Gets a vector of all the Client objects.
     * @return The client objects.
     */
    [[nodiscard]] std::vector<Client> clients();

public:
    /**
   *  Construct a Server that listens for connections on the given port.
   *  The onConnect and onDisconnect arguments are callbacks called when a
   *  Client connects or disconnects from the Server respectively.
   *
   *  The callbacks can be functions, function pointers, lambdas, or any other
   *  callable construct. They should support the signature:
   *      void onConnect(Connection c);
   *      void onDisconnect(Connection c);
   *  The Connection class is an identifier for each connected Client. It
   *  contains an ID that is guaranteed to be unique across all active
   *  connections.
   *
   *  The httpMessage is a string containing HTML content that will be sent
   *  in response to standard HTTP requests for any path ending in `index.html`.
   */
    Server(const arepa::networking::websocket::Options& options,
        std::function<void(std::shared_ptr<Connection>)> onConnect,
        std::function<void(std::shared_ptr<Connection>)> onDisconnect)
        : _listener(options) {

        this->_listener.start();
        this->_listener.on_accept([this, onConnect, onDisconnect](std::shared_ptr<arepa::networking::Socket> raw_socket) {
            auto connection = make_shared<Connection>(raw_socket);
            auto ptr = weak_ptr<Connection>(connection);

            connection->socket().on_close([this, ptr, onConnect, onDisconnect]() {
                auto connection = ptr.lock();
                onDisconnect(connection);

                {
                    std::unique_lock guard(this->_connections_lock);
                    this->_connections.erase(
                        std::find_if(std::begin(this->_connections), std::end(this->_connections),
                            [&connection](
                                Server::Client client) {
                                return client.connection == connection;
                            }));
                }
            });

            {
                std::unique_lock guard(this->_connections_lock);
                this->_connections.push_back({ connection,
                    connection->create_packet_queue() });
            }

            // Call the callback...
            onConnect(connection);
        });
    }

    /**
   *  Send a list of messages to their respective Clients.
   */
    void send(const std::deque<Message>& messages);

    /**
   *  Receive Message instances from Client instances. This returns all Message
   *  instances collected by previous calls to Server::update() and not yet
   *  received.
   */
    [[nodiscard]] std::deque<Message> receive();

    /**
     * Gets a vector of all the connections.
     * @return The connections.
     */
    [[nodiscard]] std::vector<std::shared_ptr<Connection>> connections();

    /**
     * Gets a connection by its ID.
     * @param id The connection ID.
     * @return The connection object.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Connection>> connection(ConnectionId id);

    /**
   *  Disconnect the Client specified by the given Connection.
   */
    void disconnect(ConnectionId id);
};


}

// This is required to use a ConnectionId as an unordered map key.
template <>
struct std::hash<networking::ConnectionId> {
    std::size_t operator()(const networking::ConnectionId& id) const {
        return boost::uuids::hash_value(id.uuid);
    }
};

#endif
