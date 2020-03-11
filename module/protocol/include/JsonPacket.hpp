#pragma once

#include "PacketException.hpp"

#include <arepa/networking/Socket.hpp>
#include <arepa/protocol/Message.hpp>

#include <google/protobuf/any.pb.h>
#include <memory>

namespace arepa::protocol {

/**
 * A wrapper around a network packet.
 */
class JsonPacket {

#pragma mark - Fields -
private:
    google::protobuf::Any _packet;


#pragma mark - Constructors -
public:
    JsonPacket(google::protobuf::Any packet);    // NOLINT

    JsonPacket(const google::protobuf::Message& packet);    // NOLINT


#pragma mark - Methods -
public:
    /**
     * Gets the packet type URL.
     * @return The packet type URL.
     */
    std::string type() const;

    /**
     * Checks to see if this packet is of type T.
     * @tparam T The protobuf message type.
     * @return True if the packet is of T.
     */
    template <typename T>
    bool is() const {
        std::string type_name;
        google::protobuf::Any::ParseAnyTypeUrl(this->_packet.type_url(), &type_name);
        return type_name == T::GetDescriptor()->full_name();
    }

    /**
     * Tires to cast the packet to type T.
     * Technically, this is unpacking the Any.
     *
     * @tparam T The protobuf message type.
     * @return The packet, or nullopt if it's the wrong type.
     */
    template <typename T>
    std::optional<T> try_downcast() const noexcept(false) {
        if (!this->is<T>()) {
            return std::nullopt;
        }

        // Unpack the Any.
        T unpacked;
        this->_packet.UnpackTo(&unpacked);
        return { unpacked };
    };

    /**
     * Casts the packet to type T.
     * Technically, this is unpacking the Any.
     *
     * @tparam T The protobuf message type.
     * @return The packet.
     * @throws PacketException When T doesn't match the packet type.
     */
    template <typename T>
    T downcast() const noexcept(false) {
        auto maybe = this->try_downcast<T>();
        if (!maybe) {
            throw PacketException(PacketException::ILLEGAL_CAST);
        }

        return *maybe;
    };


#pragma mark - Operators -

    template <typename T>
    operator T() {    // NOLINT
        return this->downcast<T>();
    }


#pragma mark - Serialization -
public:
    static JsonPacket from_bytes(const arepa::networking::Socket::Data& data);
    static arepa::networking::Socket::Data to_bytes(const JsonPacket& data);
    static arepa::networking::Socket::Data to_bytes(const google::protobuf::Any& data);
};
}
