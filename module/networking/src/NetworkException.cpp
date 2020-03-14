#include "NetworkException.hpp"

using namespace arepa::networking;
using Kind = NetworkException::Kind;

constexpr const char* what_message(Kind kind) noexcept {
    switch (kind) {
    case Kind::ESTABLISH_ERROR:
        return "Failed to establish a socket.";
    case Kind::BIND_ERROR:
        return "Failed to bind the server listener.";
    default:
        return "Some error";    // TODO(ethan): Error handling.
    }
}

NetworkException::NetworkException(Kind kind, std::string details)
    : _kind(kind)
    , _details(std::move(details)) {
    this->_what = std::string(what_message(kind)) + " (" + *(this->details()) + ")";
}

NetworkException::NetworkException(Kind kind)
    : _kind(kind)
    , _what(std::string(what_message(kind))) {}

Kind NetworkException::kind() const noexcept {
    return this->_kind;
}

const char* NetworkException::what() const noexcept {
    return this->_what.c_str();
}

std::optional<std::string> NetworkException::details() const noexcept {
    return this->_details;
}
