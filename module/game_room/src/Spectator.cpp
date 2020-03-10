#include "Spectator.hpp"

using namespace arepa::game::room;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Spectator::Spectator(std::shared_ptr<Player> player)
    : player(std::move(player))
    , can_join(true) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool Spectator::operator==(const Spectator& other) const {
    return this->player == other.player;
}

bool Spectator::operator==(const Player& other) const {
    return this->player == other;
}

bool Spectator::operator!=(const Spectator& other) const {
    return !(*this == other);
}

bool Spectator::operator!=(const Player& other) const {
    return !(*this == other);
}
