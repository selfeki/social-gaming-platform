#pragma once

#include "PacketException.hpp"

#include <arepa/networking/Socket.hpp>
#include <arepa/protocol/Message.hpp>

#include <google/protobuf/any.pb.h>
#include <memory>

namespace arepa::server {

/**
 * A wrapper around a network packet.
 */
class Packet {

#pragma mark - Fields -
private:
    google::protobuf::Any _packet;


#pragma mark - Constructors -
public:
    Packet(google::protobuf::Any packet);    // NOLINT

    Packet(const google::protobuf::Message& packet);    // NOLINT


#pragma mark - Methods -
public:
    /**
     * Checks to see if this packet is of type T.
     * @tparam T The protobuf message type.
     * @return True if the packet is of T.
     */
    template <typename T>
    bool is() {
        std::string type_name;
        google::protobuf::Any::ParseAnyTypeUrl(this->_packet.type_url(), &type_name);
        return type_name == T::GetDescriptor()->full_name();
    };

#pragma mark - Operators -

    template <typename T>
    operator T() {    // NOLINT
        if (!this->is<T>()) {
            throw PacketException(PacketException::ILLEGAL_CAST);
        }

        // Unpack the Any.
        T unpacked;
        this->_packet.UnpackTo(&unpacked);
        return unpacked;
    }


#pragma mark - Serialization -
public:
    static Packet from_bytes(const arepa::networking::Socket::Data& data);
    static arepa::networking::Socket::Data to_bytes(const Packet& data);
    static arepa::networking::Socket::Data to_bytes(const google::protobuf::Any& data);
};
}
