#pragma once

#include <arepa/command/Executor.hpp>
#include <arepa/game/Player.hpp>
#include <arepa/game/Room.hpp>
#include <arepa/game/RoomId.hpp>
#include <arepa/game_spec/GameSpecification.h>
#include <arepa/server/Connection.hpp>
#include <arepa/server_config/Config.h>

#include <exception>
#include <memory>
#include <optional>
#include <unordered_map>


//namespace arepa::game { // TODO(somebody): Move all the other files in this module into a namespace like this.

/**
 * The game manager.
 */
class GameManager {
#pragma mark - Types -
public:
    using Player = arepa::game::Player;
    using PlayerId = arepa::game::Player::Id;
    using Room = arepa::game::Room;
    using RoomId = arepa::game::Room::Id;

#pragma mark - Fields -
private:
    std::unordered_map<PlayerId, std::shared_ptr<Player>> _playerid_to_player;
    std::unordered_map<PlayerId, std::shared_ptr<Room>> _playerid_to_room;
    std::unordered_map<RoomId, std::shared_ptr<Room>> _roomid_to_room;

#pragma mark - Constructors -
public:
    /**
     * Creates a new game manager.
     * @param config The server configuration.
     */
    explicit GameManager(const serverConfig::Configuration& config);


#pragma mark - Methods (Static) -
public:
    /**
     * Create a Player shared pointer from a client connection.
     * This will not prevent you from creating multiple copies.
     *
     * @param connection The client connection.
     * @return The player shared pointer.
     */
    static std::shared_ptr<Player> make_player(const std::shared_ptr<arepa::server::Connection>& connection);


#pragma mark - Methods (Public) -
public:
    /**
     * Creates a new game room.
     * @param creator The user who created the room.
     * @return The newly-created room.
     */
    std::shared_ptr<Room> create_room();

    /**
     * Destroys a game room.
     * @param room The game room to destroy.
     */
    void destroy_room(const std::shared_ptr<Room>& room);

    /**
     * Adds a player to a room.
     *
     * @param player The player that will join the room.
     * @param room The room to join.
     */
    void player_join_room(const std::shared_ptr<Player>& player, const std::shared_ptr<Room>& room);

    /**
     * Removes a player from a room.
     * 
     * @param player The player that will leave the room.
     * @param room The room to leave.
     */
    void player_leave_room(const std::shared_ptr<Player>& player, const std::shared_ptr<Room>& room);

    /**
     * Sets a player's nickname.
     *
     * @param player The player.
     * @param name The player's new nickname.
     *
     * @throws GameException::USER_NICKNAME_TAKEN If the user's nickname was taken.
     */
    void player_set_nickname(const std::shared_ptr<Player>& player, const Player::Name& name);

    /**
     * Finds the room that a given player is inside.
     * 
     * @param player The player.
     * @return The room, or nullopt if they aren't in a room.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Room>> find_player_room(const std::shared_ptr<Player>& player);

    /**
     * Finds the room that a given player is inside.
     *
     * @param player The player ID.
     * @return The room, or nullopt if they aren't in a room.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Room>> find_player_room(const PlayerId& player);

    /**
     * Finds the room with a given room ID.
     *
     * @param room The room ID.
     * @return The room, or nullopt if no such room exists.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Room>> find_room(const RoomId& room);

    /**
     * Finds the Player from a player ID.
     *
     * @param player The player ID.
     * @return The player object, or nullopt if they don't have an associated object.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Player>> find_player(const PlayerId& player);

    /**
     * Adds a Player object to the player map.
     * @param player The player object.
     */
    void add_player(const std::shared_ptr<Player>& player);

    /**
     * Removes a Player object from the player map.
     * This will also kick them from their room.
     * 
     * @param player The player object.
     */
    void remove_player(const std::shared_ptr<Player>& player);
};

// }
