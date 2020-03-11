#include "GameException.hpp"

using namespace arepa::game;
using Kind = GameException::Kind;

constexpr const char* what_message(Kind kind) noexcept {
    switch (kind) {
    case Kind::USER_NOT_FOUND:
        return "The specified user is not in the game room.";
    case Kind::USER_NICKNAME_TAKEN:
        return "The specified nickname is already in use.";
    case Kind::PLAYER_IS_SPECTATOR:
        return "That user is already a spectator in the room. Remove them as a spectator first.";
    case Kind::SPECTATOR_IS_PLAYER:
        return "That user is already a player in the room. Remove them as a player first.";
    case Kind::ROOM_FULL:
        return "The room is full.";
    case Kind::ROOM_NOT_FOUND:
        return "The specified room code is invalid.";
    case Kind::NOT_ALLOWED_DURING_GAME:
        return "You can't do that during a game.";
    default:
        return "Some error";    // TODO(ethan): Error handling.
    }
}

GameException::GameException(Kind kind, std::string details)
    : _kind(kind)
    , _details(std::move(details)) {
    this->_what = std::string(what_message(kind)) + " (" + *(this->details()) + ")";
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
