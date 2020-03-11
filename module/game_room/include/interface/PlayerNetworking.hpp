#pragma once

#include <arepa/protocol/Packet.hpp>

#include <string_view>

namespace arepa::game::room {

/**
 * An interface for sending data to players.
 */
class PlayerNetworking {
#pragma mark - Types -
public:
    using Packet = arepa::protocol::Packet;


#pragma mark - Constructors -
public:
    virtual ~PlayerNetworking() = default;


#pragma mark - Virtual Methods -
public:
    /**
     * Sends the player a text message.
     * @param message The message to send.
     */
    virtual void send_message(const std::string_view& message) = 0;

    /**
     * Sends the player a raw packet.
     * @param packet The packet to send.
     */
    virtual void send_packet(const Packet& packet) = 0;
};

}
