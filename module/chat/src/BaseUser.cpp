#include "internal/BaseUser.hpp"

using namespace arepa::chat;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool BaseUser::operator==(const BaseUser& other) const {
    return this->id() == other.id();
}

bool BaseUser::operator==(const BaseUser::Id& other) const {
    return this->id() == other;
}

bool BaseUser::operator!=(const BaseUser& other) const {
    return !(*this == other);
}

bool BaseUser::operator!=(const BaseUser::Id& other) const {
    return !(*this == other);
}
