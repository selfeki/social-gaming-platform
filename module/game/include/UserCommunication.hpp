#pragma once

#include <arepa/protocol/Packet.hpp>

#include <string_view>

namespace arepa::game {

/**
 * An interface for sending data to users.
 */
class UserCommunication {
#pragma mark - Types -
public:
    using Packet = arepa::protocol::Packet;


#pragma mark - Constructors -
public:
    virtual ~UserCommunication() = default;


#pragma mark - Virtual Methods -
public:
    /**
     * Sends the player a raw text message.
     * @param message The message to send.
     */
    virtual void send_message(const std::string_view& message) = 0;

    /**
     * Sends the player an error message.
     * @param message The message to send.
     */
    virtual void send_error_message(const std::string_view& message) = 0;

    /**
     * Sends the player a system message.
     * @param message The message to send.
     */
    virtual void send_system_message(const std::string_view& message) = 0;

    /**
     * Sends the player a raw packet.
     * @param packet The packet to send.
     */
    virtual void send_packet(const Packet& packet) = 0;
};

}
