#pragma once

#include "PlayerStats.hpp"
#include "User.hpp"

#include <optional>

namespace arepa::game {

/**
 * An interface representing a spectator.
 */
class Spectator : public virtual User {

#pragma mark - Types -
public:
    using Id = User::Id;
    using Name = User::Name;


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const Spectator& other) const;
    [[nodiscard]] bool operator!=(const Spectator& other) const;
};

}
