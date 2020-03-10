#pragma once

#include "Player.hpp"

#include <memory>

namespace arepa::game::room {

/**
 * A spectator.
 * This is a player that doesn't participate.
 */
class Spectator {
#pragma mark - Types -
public:
    using Id = Player::Id;

#pragma mark - Fields (Public) -
public:
    /**
     * The spectator's std::shared_ptr<Player> object.
     */
    std::shared_ptr<Player> player;

    /**
     * Whether or not the spectator can join a game.
     */
    bool can_join;


#pragma mark - Constructors -
public:
    /**
     * Creates a new player instance.
     * @param id The player's session ID.
     */
    Spectator(std::shared_ptr<Player> player);


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const Spectator& other) const;
    [[nodiscard]] bool operator==(const Player& other) const;

    [[nodiscard]] bool operator!=(const Spectator& other) const;
    [[nodiscard]] bool operator!=(const Player& other) const;
};

}
