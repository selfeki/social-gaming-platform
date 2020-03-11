#pragma once

#include "Player.hpp"
#include "RoomId.hpp"

#include <arepa/command/Command.hpp>
#include <arepa/command/CommandMap.hpp>
#include <arepa/command/Executor.hpp>

#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

namespace arepa::game::room {

/**
 * A game room.
 * This is a room that allows users to chat and play games together.
 */
class Room {
#pragma mark - Types -
public:
    using Id = RoomId;
    using CommandMap = arepa::command::CommandMap<Player&, Room&>;
    using CommandExecutor = CommandMap::Executor;

    enum class LimitBehavior {
        /**
         * Excess players will be kicked.
         */
        KICK,

        /**
         * Excess players will be converted to spectators.
         */
        SPECTATE
    };


#pragma mark - Fields -
private:
    std::optional<std::shared_ptr<Player>> _owner;
    std::chrono::system_clock::time_point _created_at;
    Id _id;

    size_t _player_limit;
    std::vector<std::shared_ptr<Player>> _players;
    std::vector<std::shared_ptr<Player>> _spectators;

    // GameInstance _game;


#pragma mark - Fields (Public) -
public:
    CommandMap commands;


#pragma mark - Constructors -
public:
    /**
     * Creates a new game room.
     * The first player added to this room will be set as the owner.
     *
     * @param id The room ID.
     */
    Room(Id id);

    /**
     * Creates a new game room with a random room ID.
     * The first player added to this room will be set as the owner.
     */
    Room();


#pragma mark - Methods (Private) -

    /**
     * Joins a spectator into the room.
     */
    void _join_spectator();

    /**
     * Finds a player or spectator in the room.
     * @param name_or_id The player name or ID.
     * @return The player pointer, or nullopt.
     */
    template <typename T>
    std::optional<std::shared_ptr<Player>> _find_player_or_spectator(T name_or_id) {
        auto foundPlayer = this->find_player(name_or_id);
        if (foundPlayer) {
            return *foundPlayer;
        }

        auto foundSpectator = this->find_spectator(name_or_id);
        if (foundSpectator) {
            return *foundSpectator;
        }

        return std::nullopt;
    }


#pragma mark - Methods -
public:
    /**
     * Gets the room ID.
     * @return The room ID.
     */
    [[nodiscard]] const Id& id() const;

    /**
     * Gets the maximum number of players in this room.
     * @return The maximum number of players.
     */
    [[nodiscard]] size_t player_limit() const;

    /**
     * Gets the players in this room.
     * @return The players in this room.
     */
    [[nodiscard]] const std::vector<std::shared_ptr<Player>>& players() const;

    /**
     * Gets the players currently playing a game.
     * This may be different than the players in the room (i.e. when a spectator joins an in-progress game).
     *
     * @return The players currently playing a game.
     */
    [[nodiscard]] const std::vector<std::shared_ptr<Player>>& players_in_game() const;

    /**
     * Gets the spectators in this room.
     * @return The spectators in this room.
     */
    [[nodiscard]] const std::vector<std::shared_ptr<Player>>& spectators() const;

    /**
     * Gets the date when the room was created.
     * @return The room creation date.
     */
    [[nodiscard]] const std::chrono::system_clock::time_point& creation_date() const;

    /**
     * Sets the player limit.
     * @param limit The new player limit.
     * @param behavior What to do with excess players that exceed the limit.
     */
    void set_player_limit(size_t limit, LimitBehavior behavior);

    /**
     * Removes a player from the room.
     * @param player The player to remove.
     */
    void remove_player(const Player& player);

    /**
     * Removes a player from the room.
     * If the owner leaves, the oldest player will be made the new owner.
     *
     * @param player The ID of the player to remove.
     */
    void remove_player(Player::Id player);

    /**
     * Removes a spectator from the room.
     * @param spectator The spectator to remove.
     */
    void remove_spectator(const Player& spectator);

    /**
     * Removes a spectator from the room.
     * @param spectator The spectator to remove.
     */
    void remove_spectator(Player::Id spectator);

    /**
     * Adds a player to the room.
     * If no other players are in the room, they will become the owner.
     *
     * @throws GameException::ROOM_FULL When the room is full.
     * @param player The player to add.
     */
    void add_player(const std::shared_ptr<Player>& player);

    /**
     * Adds a spectator to the room.
     * @param spectator The spectator to add.
     * @param waitlist The spectator can be added to the game when a player leaves.
     */
    void add_spectator(const std::shared_ptr<Player>& spectator, bool waitlist);

    /**
     * Gets the room owner.
     * @return The room owner. This will be nullopt when the room is empty.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Player>> owner() const;

    /**
     * Finds a player object in this room.
     *
     * @param player The player ID.
     * @return The player object, or nullopt if not found.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Player>> find_player(Player::Id player) const;

    /**
     * Finds a spectator object in this room.
     *
     * @param player The spectator ID.
     * @return The spectator object, or nullopt if not found.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Player>> find_spectator(Player::Id spectator) const;

    /**
     * Finds a player object in this room.
     *
     * @param player The player name.
     * @return The player object, or nullopt if not found.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Player>> find_player(Player::Name player) const;

    /**
     * Finds a spectator object in this room.
     *
     * @param player The spectator name.
     * @return The spectator object, or nullopt if not found.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Player>> find_spectator(Player::Name spectator) const;

    /**
     * Checks if the room is full.
     * @return True if the room is full.
     */
    [[nodiscard]] bool is_full() const;

    /**
     * Broadcasts a message to all players and spectators in the room.
     * @param message The message to broadcast.
     */
    void broadcast_message(const std::string& message);

    // TODO(ethan): broadcast_packet

    /**
     * Sets the room owner.
     * @throws GameException::PLAYER_NOT_FOUND
     * @param owner The new owner.
     */
    void set_owner(std::shared_ptr<Player> owner);

    /**
     * Runs a tick on the game instance.
     */
    void update_game();

    /**
     * Processes a player or spectators's message.
     *
     * @param player  The player or spectator.
     * @param message The message sent by the player or spectator.
     *
     * @return True if the message was handled by the room.
     */
    bool process_message(Player::Id player, const std::string& message);

    /**
     * Processes a player or spectator's command.
     *
     * @param player The player or spectator.
     * @param command  The command sent by the player or spectator.
     *
     * @return True if the command was handled by the room.
     */
    bool process_command(Player::Id player, const arepa::command::Command& command);


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const Room& other) const;
    [[nodiscard]] bool operator!=(const Room& other) const;
};

}
