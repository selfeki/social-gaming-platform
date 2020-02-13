#include "NetworkException.hpp"

using namespace arepa::networking;
using Kind = NetworkException::Kind;

NetworkException::NetworkException(Kind kind)
    : _kind(kind) {}

const char* NetworkException::what() const noexcept {
    switch (this->_kind) {
    case Kind::ESTABLISH_ERROR:
        return "Failed to establish a socket.";
    case Kind::BIND_ERROR:
        return "Failed to bind the server listener.";
    default:
        return "Some error";    // TODO(ethan): Error handling.
    }
}

Kind NetworkException::kind() const noexcept {
    return this->_kind;
}
