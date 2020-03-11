#pragma once

#include "PacketException.hpp"

#include <arepa/networking/Socket.hpp>

#include <memory>

namespace arepa::protocol {

/**
 * A string.
 * It's literally just a string.
 */
class TextPacket {

#pragma mark - Fields -
public:
    std::string text;


#pragma mark - Constructors -
public:
    TextPacket(std::string text);    // NOLINT


#pragma mark - Operators -

    operator std::string() const;
    std::string* operator->();
    std::string* operator*();
    const std::string* operator->() const;
    const std::string* operator*() const;


#pragma mark - Serialization -
public:
    static TextPacket from_bytes(const arepa::networking::Socket::Data& data);
    static arepa::networking::Socket::Data to_bytes(const TextPacket& data);
};
}
