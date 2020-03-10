#pragma once

#include "Player.hpp"
#include "RoomId.hpp"
#include "Spectator.hpp"

#include <chrono>
#include <memory>
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
    std::vector<Spectator> _spectators;

    // GameInstance _game;


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
    [[nodiscard]] const std::vector<Spectator>& spectators() const;

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
    void remove_player(const std::shared_ptr<Player>& player);

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
    void remove_spectator(const Spectator& spectator);

    /**
     * Removes a spectator from the room.
     * @param spectator The spectator to remove.
     */
    void remove_spectator(Spectator::Id spectator);

    /**
     * Adds a player to the room.
     * If no other players are in the room, they will become the owner.
     *
     * @throws GameException::ROOM_FULL When the room is full.
     * @param player The player to add.
     */
    void add_player(std::shared_ptr<Player> player);

    /**
     * Adds a spectator to the room.
     * @param spectator The spectator to add.
     */
    void add_spectator(Spectator spectator);

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
    [[nodiscard]] std::optional<std::shared_ptr<Player>> find_player(Player::Id player);

    /**
     * Finds a player object in this room.
     *
     * @param player The player name.
     * @return The player object, or nullopt if not found.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Player>> find_player(Player::Name player);

    /**
     * Checks if the room is full.
     * @return True if the room is full.
     */
    [[nodiscard]] bool is_full() const;

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


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const Room& other) const;
    [[nodiscard]] bool operator!=(const Room& other) const;
};

}
