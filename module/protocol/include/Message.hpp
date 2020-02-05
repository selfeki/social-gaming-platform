#pragma once

#include <arepa/networking/Socket.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace arepa::protocol {

/**
 * A simple protocol that converts raw bytes to strings.
 */
class Message {

#pragma mark - Fields -
public:
    /**
     * The message text.
     */
    std::string text;


#pragma mark - Constructor -
public:
    /**
     * Creates a new message from a string.
     * @param text The message text.
     */
    Message(std::string text);    // NOLINT


#pragma mark - Serialization -
public:
    /**
     * Deserializes bytes into a message.
     * 
     * @param bytes The raw message bytes.
     * @return The deserialized messages.
     */
    static Message from_bytes(const std::vector<std::uint8_t>& bytes);

    /**
     * Serializes a message into bytes.
     *
     * @param message The message string.
     * @return The serialized bytes.
     */
    static std::vector<std::uint8_t> to_bytes(const std::string& message);

    /**
     * Serializes a message into bytes.
     *
     * @param message The message object.
     * @return The serialized bytes.
     */
    static std::vector<std::uint8_t> to_bytes(const Message& message);


#pragma mark - Operators -
public:
    operator std::string();    // NOLINT
};

}
