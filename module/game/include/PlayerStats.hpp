#pragma once

#include <arepa/networking/SessionId.hpp>

namespace arepa::game {

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
     * Returns a mutable reference to the number of times a player has won.
     * @return A reference to the number of player wins.
     */
    [[nodiscard]] unsigned int& wins();

    /**
     * Returns the number of times a player has participated a game.
     * @return The number of player games.
     */
    [[nodiscard]] unsigned int games() const;

    /**
     * Returns a mutable reference to the number of times a player has participated in a game.
     * @return A reference to the number of player games.
     */
    [[nodiscard]] unsigned int& games();
};

}
