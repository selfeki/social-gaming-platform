#pragma once

#include "Environment.hpp"

#include <arepa/Result.hpp>
#include <arepa/chat/Player.hpp>
#include <arepa/chat/Spectator.hpp>
#include <arepa/command/Command.hpp>

namespace arepa::game {

/**
 * An interface representing a game instance.
 */
class Controller {

#pragma mark - Types -
public:
    using Player = arepa::chat::Player;
    using Spectator = arepa::chat::Spectator;
    using Command = arepa::command::Command;

    using OptionKey = std::string;
    using OptionValue = std::string;

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


#pragma mark - Constructors -
public:
    virtual ~Controller() = default;


#pragma mark - Methods -
public:
    /**
     * Called to initialize the game instance.
     * @param environment The game environment.
     */
    virtual void initialize(Environment& environment) = 0;

    /**
     * Intercepts a player's message.
     * This can be used to prevent default behavior, or implement custom actions.
     *
     * @param player  The player.
     * @param message The message sent by the player.
     *
     * @return True if the message was handled by the room.
     */
    virtual Intercept intercept_player_message(Player& player, const std::string& message) = 0;

    /**
     * Intercepts a player's command.
     * This can be used to prevent default behavior, or implement custom commands.
     *
     * @param player   The player.
     * @param command  The command sent by the player.
     *
     * @return True if the command was handled by the room.
     */
    virtual Intercept intercept_player_command(Player& player, const Command& command) = 0;

    /**
     * Intercepts a spectator's message.
     * This can be used to prevent default behavior, or implement custom actions.
     *
     * @param player  The spectator.
     * @param message The message sent by the spectator.
     *
     * @return True if the message was handled by the room.
     */
    virtual Intercept intercept_spectator_message(Spectator& spectator, const std::string& message) = 0;

    /**
     * Intercepts a spectator's command.
     * This can be used to prevent default behavior, or implement custom commands.
     *
     * @param spectator The spectator.
     * @param command  The command sent by the spectator.
     *
     * @return True if the command was handled by the room.
     */
    virtual Intercept intercept_spectator_command(Spectator& spectator, const Command& command) = 0;

    /**
     * Called when a player quits the game.
     * This can be used to cancel the game or change the behavior.
     *
     * @param player The player that quit.
     */
    virtual void on_player_quit(Player& player) = 0;

    /**
     * Called when a game option changes.
     * 
     * @param option The option name.
     * @param value The option value.
     */
    virtual arepa::Result<void, std::string> on_option_change(const OptionKey& option, const OptionValue& value) = 0;

    /**
     * Gets a list of option names and their descriptions.
     * This can be provided to the player as a way to help them configure the game.
     *
     * @return A vector.
     */
    virtual std::vector<std::pair<OptionKey, std::string>> list_option_descriptions();

    /**
     * Update the game by one tick.
     */
    virtual void update() = 0;

    /**
     * Called to reset and start the game.
     */
    virtual void start() = 0;
};

}
