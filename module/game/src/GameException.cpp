#include "GameException.hpp"

using namespace arepa::game;
using Kind = GameException::Kind;

constexpr const char* what_message(Kind kind) noexcept {
    switch (kind) {
    case Kind::OTHER:
        return "Other error.";
    default:
        return "Some error";
    }
}

GameException::GameException(Kind kind, std::string details)
    : _kind(kind)
    , _details(std::move(details)) {
    if (kind == Kind::OTHER) {
        this->_what = std::string(*this->details());
    } else {
        this->_what = std::string(what_message(kind)) + " (" + *(this->details()) + ")";
    }
}

GameException::GameException(Kind kind)
    : _kind(kind)
    , _what(std::string(what_message(kind))) {}

Kind GameException::kind() const noexcept {
    return this->_kind;
}

const char* GameException::what() const noexcept {
    return this->_what.c_str();
}

std::optional<std::string> GameException::details() const noexcept {
    return this->_details;
}
