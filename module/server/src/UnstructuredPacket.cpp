#include "UnstructuredPacket.hpp"

#include <google/protobuf/util/json_util.h>
#include <string>

using namespace arepa::server;

using Data = arepa::networking::Socket::Data;

using std::string;
using std::vector;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

UnstructuredPacket::UnstructuredPacket(string text)
    : text(std::move(text)) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

string* UnstructuredPacket::operator*() {
    return &this->text;
}

string* UnstructuredPacket::operator->() {
    return &this->text;
}

const string* UnstructuredPacket::operator*() const {
    return &this->text;
}

const string* UnstructuredPacket::operator->() const {
    return &this->text;
}

UnstructuredPacket::operator string() const {
    return this->text;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Serialization -
// ---------------------------------------------------------------------------------------------------------------------

UnstructuredPacket UnstructuredPacket::from_bytes(const Data& bytes) {
    return string(reinterpret_cast<const char*>(bytes.data()), bytes.size());
}

Data UnstructuredPacket::to_bytes(const UnstructuredPacket& packet) {
    return vector<uint8_t>(packet.text.begin(), packet.text.end());
}
