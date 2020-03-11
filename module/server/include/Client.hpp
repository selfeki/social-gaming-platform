#pragma once

#include "Connection.hpp"

#include <arepa/communication/ChannelSingleQueue.hpp>
#include <arepa/protocol/Packet.hpp>

namespace arepa::server {

/**
 * A container struct that holds a client connection, and a client packet queue.
 */
class Client {

#pragma mark - Fields (Public) -
public:
    std::shared_ptr<Connection> connection;
    std::shared_ptr<arepa::communication::ChannelSingleQueue<arepa::protocol::Packet>> messages;    // NOTE: This is a pointer.

#pragma mark - Constructors -
public:
    explicit Client(std::shared_ptr<Connection> connection);
};


}
