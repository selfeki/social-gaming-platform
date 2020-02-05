#include "SessionException.hpp"

using namespace arepa::networking;
using Kind = SessionException::Kind;

SessionException::SessionException(Kind kind)
    : _kind(kind) {}

const char* SessionException::what() const noexcept {
    switch (this->_kind) {
    case Kind::INVALID_SESSION:
    case Kind::INVALID_SESSION_SECRET:
        return "Invalid session information.";
    }
}

Kind SessionException::kind() const noexcept {
    return this->_kind;
}
