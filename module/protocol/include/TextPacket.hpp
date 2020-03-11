#pragma once

#include "PacketException.hpp"

#include <arepa/networking/Socket.hpp>

#include <memory>

namespace arepa::protocol {

/**
 * A string.
 * It's literally just a string.
 */
class UnstructuredPacket {

#pragma mark - Fields -
public:
    std::string text;


#pragma mark - Constructors -
public:
    UnstructuredPacket(std::string text);    // NOLINT


#pragma mark - Operators -

    operator std::string() const;
    std::string* operator->();
    std::string* operator*();
    const std::string* operator->() const;
    const std::string* operator*() const;


#pragma mark - Serialization -
public:
    static UnstructuredPacket from_bytes(const arepa::networking::Socket::Data& data);
    static arepa::networking::Socket::Data to_bytes(const UnstructuredPacket& data);
};
}
