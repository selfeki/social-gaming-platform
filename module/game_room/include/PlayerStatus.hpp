#pragma once

#include "PlayerNickname.hpp"
#include "PlayerStats.hpp"

#include <arepa/networking/SessionId.hpp>

#include <optional>

namespace arepa::game::room {

/**
 * A player's game Room status.
 */
enum class PlayerStatus : int {

    /**
     * The player is a spectator.
     */
    SPECTATOR = 0b0001,

    /**
     * The player is a spectator who is waiting to join the game.
     */
    WAITLIST = 0b0011,

    /**
     * The player is in game.
     */
    PLAYING = 0b0100,

};

}
