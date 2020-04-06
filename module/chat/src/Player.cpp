#include "Player.hpp"

using namespace arepa::chat;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool Player::operator==(const Player& other) const {
    return this->id() == other.id();
}

bool Player::operator!=(const Player& other) const {
    return !(*this == other);
}
