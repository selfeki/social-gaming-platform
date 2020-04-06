#pragma once

#include <arepa/networking/SessionId.hpp>

namespace arepa::chat {

/**
 * A player's game statistics.
 */
class PlayerStats {
#pragma mark - Fields -
private:
    unsigned int _wins;
    unsigned int _games;


#pragma mark - Constructors -
public:
    PlayerStats();


#pragma mark - Methods -
    /**
     * Returns the number of times a player has won.
     * @return The number of player wins.
     */
    [[nodiscard]] unsigned int wins() const;

    /**
     * Returns the number of times a player has participated a game.
     * @return The number of player games.
     */
    [[nodiscard]] unsigned int games() const;

    /**
     * Increments the player's win counter.
     */
    void increment_wins();

    /**
     * Increments the player's games counter.
     */
    void increment_games();
};

}
