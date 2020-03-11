#include "PacketException.hpp"

using namespace arepa::protocol;
using Kind = PacketException::Kind;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

PacketException::PacketException(Kind kind)
    : _kind(kind) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

const char* PacketException::what() const noexcept {
    switch (this->_kind) {
    case Kind::MALFORMED:
        return "Malformed or invalid packet.";
    case Kind::ILLEGAL_CAST:
        return "Illegal conversion between packet types.";
    }
}

Kind PacketException::kind() const noexcept {
    return this->_kind;
}
