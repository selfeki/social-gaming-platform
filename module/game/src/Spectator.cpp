#include "Spectator.hpp"

using namespace arepa::game;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool Spectator::operator==(const Spectator& other) const {
    return this->id() == other.id();
}

bool Spectator::operator!=(const Spectator& other) const {
    return !(*this == other);
}
