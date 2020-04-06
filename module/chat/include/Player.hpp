#pragma once

#include "PlayerStats.hpp"
#include "internal/BaseUser.hpp"

#include <optional>

namespace arepa::chat {

/**
 * An interface representing a player.
 */
class Player : public virtual BaseUser {

#pragma mark - Types -
public:
    using Id = BaseUser::Id;
    using Name = BaseUser::Name;
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
