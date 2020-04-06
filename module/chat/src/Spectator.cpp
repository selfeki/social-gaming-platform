#include "Spectator.hpp"

using namespace arepa::chat;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool Spectator::operator==(const Spectator& other) const {
    return this->id() == other.id();
}

bool Spectator::operator!=(const Spectator& other) const {
    return !(*this == other);
}
