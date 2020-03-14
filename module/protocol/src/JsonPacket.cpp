#include "JsonPacket.hpp"

#include <google/protobuf/util/json_util.h>
#include <string>

using namespace arepa::protocol;

using std::basic_string_view;
using std::ostream;
using std::string;

using Data = arepa::networking::Socket::Data;

using google::protobuf::Any;
using google::protobuf::util::JsonStringToMessage;
using google::protobuf::util::MessageToJsonString;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

JsonPacket::JsonPacket(Any packet)
    : _packet(std::move(packet)) {}

JsonPacket::JsonPacket(const google::protobuf::Message& packet) {
    Any any;
    any.PackFrom(packet);
    this->_packet = std::move(any);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

std::string JsonPacket::type() const {
    return this->_packet.type_url();
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Serialization -
// ---------------------------------------------------------------------------------------------------------------------

JsonPacket JsonPacket::from_bytes(const Data& bytes) {
    Any packet;
    JsonStringToMessage(google::protobuf::StringPiece(reinterpret_cast<const char*>(bytes.data()), bytes.size()), &packet);

    return packet;
}

Data JsonPacket::to_bytes(const Any& packet) {
    string packetJson;
    MessageToJsonString(packet, &packetJson);

    // Return the byte vector of the JSON string.
    return std::vector<uint8_t>(packetJson.begin(), packetJson.end());
}

Data JsonPacket::to_bytes(const JsonPacket& packet) {
    return JsonPacket::to_bytes(packet._packet);
}
