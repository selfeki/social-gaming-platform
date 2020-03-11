#include "TextPacket.hpp"

#include <google/protobuf/util/json_util.h>
#include <string>

using namespace arepa::protocol;

using Data = arepa::networking::Socket::Data;

using std::string;
using std::vector;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

TextPacket::TextPacket(string text)
    : text(std::move(text)) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

string* TextPacket::operator*() {
    return &this->text;
}

string* TextPacket::operator->() {
    return &this->text;
}

const string* TextPacket::operator*() const {
    return &this->text;
}

const string* TextPacket::operator->() const {
    return &this->text;
}

TextPacket::operator string() const {
    return this->text;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Serialization -
// ---------------------------------------------------------------------------------------------------------------------

TextPacket TextPacket::from_bytes(const Data& bytes) {
    return string(reinterpret_cast<const char*>(bytes.data()), bytes.size());
}

Data TextPacket::to_bytes(const TextPacket& packet) {
    return vector<uint8_t>(packet.text.begin(), packet.text.end());
}
