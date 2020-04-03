#pragma once

#include "PlayerStats.hpp"
#include "User.hpp"

#include <optional>

namespace arepa::game {

/**
 * An interface representing a player.
 */
class Player : public virtual User {

#pragma mark - Types -
public:
    using Id = User::Id;
    using Name = User::Name;
    using Stats = PlayerStats;


#pragma mark - Methods -
public:
    /**
     * Gets the player's stats.
     * @return The player's stats object.
     */
    virtual const Stats& stats() const = 0;

    /**
     * Gets the player's stats.
     * @return The player's stats object.
     */
    virtual const Stats& stats() = 0;


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const Player& other) const;
    [[nodiscard]] bool operator!=(const Player& other) const;
};

}
