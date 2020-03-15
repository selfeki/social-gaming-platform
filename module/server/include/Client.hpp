#pragma once

#include "Connection.hpp"

#include <arepa/communication/ChannelSingleQueue.hpp>
#include <arepa/protocol/Packet.hpp>

namespace arepa::server {

/**
 * A container struct that holds a client connection, and a client packet queue.
 */
class Client {
#pragma mark - Types -
public:
    using packet_type = arepa::protocol::Packet;


#pragma mark - Fields (Public) -
public:
    std::shared_ptr<Connection> connection;
    std::shared_ptr<arepa::communication::ChannelSingleQueue<packet_type>> messages;    // NOTE: This is a pointer.


#pragma mark - Constructors -
public:
    explicit Client(std::shared_ptr<Connection> connection);
};


}
