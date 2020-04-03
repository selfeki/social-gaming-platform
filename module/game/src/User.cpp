#include "User.hpp"

using namespace arepa::game;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool User::operator==(const User& other) const {
    return this->id() == other.id();
}

bool User::operator==(const User::Id& other) const {
    return this->id() == other;
}

bool User::operator!=(const User& other) const {
    return !(*this == other);
}

bool User::operator!=(const User::Id& other) const {
    return !(*this == other);
}
