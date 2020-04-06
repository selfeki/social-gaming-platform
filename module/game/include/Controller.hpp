#pragma once

#include "Player.hpp"

#include <optional>

namespace arepa::game {

/**
 * An interface representing a game instance.
 */
class GameInstance {

#pragma mark - Types -
public:
    enum class Intercept {

        /**
         * The action was intercepted, and the default handler should not be called.
         */
        INTERCEPTED,

        /**
         * The default handler should be called.
         */
        DEFAULT

    };


#pragma mark - Methods -
public:
    /**
     * Intercepts a player's message.
     * This can be used to prevent default behavior, or implement custom actions.
     *
     * @param player  The player or spectator.
     * @param message The message sent by the player or spectator.
     *
     * @return True if the message was handled by the room.
     */
    virtual Intercept intercept_message(Player& player, const std::string& message);

    /**
     * Intercepts a player's command.
     * This can be used to prevent default behavior, or implement custom commands.
     *
     * @param player The player or spectator.
     * @param command  The command sent by the player or spectator.
     *
     * @return True if the command was handled by the room.
     */
    virtual Intercept intercept_command(Player& player, const arepa::command::Command& command);

    virtual void player_quit(Player& player)


#pragma mark - Operators -
        public : [[nodiscard]] bool
                 operator==(const Player& other) const;
    [[nodiscard]] bool operator!=(const Player& other) const;
};

}
