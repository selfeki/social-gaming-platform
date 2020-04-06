#include "../include/ChatException.hpp"

using namespace arepa::chat;
using Kind = ChatException::Kind;

constexpr const char* what_message(Kind kind) noexcept {
    switch (kind) {
    case Kind::USER_NOT_FOUND:
        return "The specified user is not in the game room.";
    case Kind::USER_NICKNAME_TAKEN:
        return "The specified nickname is already in use.";
    case Kind::ROOM_FULL:
        return "The room is full.";
    case Kind::ROOM_NOT_FOUND:
        return "The specified room code is invalid.";
    case Kind::NOT_ALLOWED_DURING_GAME:
        return "You can't do that during a game.";
    case Kind::NOT_ALLOWED_OPTION:
        return "That option is invalid.";
    default:
        return "Some error";    // TODO(ethan): Error handling.
    }
}

ChatException::ChatException(Kind kind, std::string details)
    : _kind(kind)
    , _details(std::move(details)) {
    this->_what = std::string(what_message(kind)) + " (" + *(this->details()) + ")";
}

ChatException::ChatException(Kind kind)
    : _kind(kind)
    , _what(std::string(what_message(kind))) {}

Kind ChatException::kind() const noexcept {
    return this->_kind;
}

const char* ChatException::what() const noexcept {
    return this->_what.c_str();
}

std::optional<std::string> ChatException::details() const noexcept {
    return this->_details;
}
