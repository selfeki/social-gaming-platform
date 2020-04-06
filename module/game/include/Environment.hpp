#pragma once

#include "AbstractUser.hpp"
#include "Player.hpp"
#include "RoomId.hpp"
#include "Spectator.hpp"
#include "User.hpp"

#include <arepa/command/Command.hpp>
#include <arepa/command/CommandMap.hpp>
#include <arepa/command/Executor.hpp>
#include <arepa/communication/Signal.hpp>

#include <chrono>
#include <map>
#include <memory>
#include <set>

namespace arepa::game {

/**
 * A game room.
 * This is an interface that allows a game instance to manipulate the game room.
 */
class GameRoom {
#pragma mark - Types -
public:
    using Id = RoomId;
    using User = AbstractUser;
    using Player = arepa::game::Player;
    using Spectator = arepa::game::Spectator;
    using CommandMap = arepa::command::CommandMap<AbstractUser&, Room&>;
    using CommandExecutor = CommandMap::Executor;


#pragma mark - Methods -
public:
    /**
     * Gets the number of players in the room.
     * @return The spectator count.
     */
    [[nodiscard]] virtual size_t player_count() const = 0;

    /**
     * Gets the number of spectators in the room.
     * @return The spectator count.
     */
    [[nodiscard]] virtual size_t spectator_count() const = 0;

    /**
     * Gets the players currently playing a game.
     * This may be different than the users in the room (i.e. when a spectator joins an in-progress game).
     *
     * @return The players currently playing a game.
     */
    [[nodiscard]] virtual const std::set<std::shared_ptr<Player>>& players() const = 0;

    /**
     * Gets the spectators in this room.
     * @return The spectators in this room.
     */
    [[nodiscard]] virtual const std::set<std::shared_ptr<Spectator>>& spectators() const = 0;

    /**
     * Gets the room owner.
     * @return The room owner. This will be nullptr when the room is empty.
     */
    [[nodiscard]] virtual std::shared_ptr<User> owner() const = 0;

    /**
     * Checks if the user is a player in the room.
     * 
     * @param user The user pointer.
     * @return True if the user is a player.
     */
    [[nodiscard]] virtual bool is_player(const std::shared_ptr<User>& user) const = 0;

    /**
     * Checks if the user is a spectating the current game.
     * Users are only considered to be spectating if they are a spectator, AND a game is running.
     *
     * @param user The user pointer.
     * @return True if the user is spectating the game.
     */
    [[nodiscard]] virtual bool is_spectating(const std::shared_ptr<User>& user) const = 0;

    /**
     * Finds a player in this room.
     *
     * @param player The player name.
     * @return The player pointer, or nullopt if not found.
     */
    [[nodiscard]] virtual std::shared_ptr<Player> find_player(User::Name player) const = 0;

    /**
     * Checks if the room is full.
     * @return True if the room is full.
     */
    [[nodiscard]] virtual bool is_full() const = 0;

    /**
     * Broadcasts a message to all players and spectators in the room.
     * @param message The message to broadcast.
     */
    virtual void broadcast_message(const std::string& message) = 0;

    /**
     * Broadcasts a message to all players and spectator in the room.
     * @param message The message to broadcast.
     */
    virtual void broadcast_message_to_spectators(const std::string& message) = 0;

    /**
     * Broadcasts a packet to all players and spectators in the room.
     * @param packet The packet to broadcast.
     */
    virtual void broadcast_packet(const User::Packet& packet) = 0;

    /**
     * Broadcasts a packet to all spectators in the room.
     * @param packet The packet to broadcast.
     */
    virtual void broadcast_packet_to_spectators(const User::Packet& packet) = 0;
};

}
