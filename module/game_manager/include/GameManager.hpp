#pragma once

#include <arepa/command/Executor.hpp>
#include <arepa/game/Room.hpp>
#include <arepa/game/RoomId.hpp>
#include <arepa/game/User.hpp>
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
    using User = arepa::game::User;
    using UserId = arepa::game::User::Id;
    using Room = arepa::game::Room;
    using RoomId = arepa::game::Room::Id;

#pragma mark - Fields -
private:
    std::unordered_map<UserId, std::shared_ptr<User>> _userid_to_user;
    std::unordered_map<UserId, std::shared_ptr<Room>> _userid_to_room;
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
     * Create a User shared pointer from a client connection.
     * This will not prevent you from creating multiple copies.
     *
     * @param connection The client connection.
     * @return The user shared pointer.
     */
    static std::shared_ptr<User> make_user(const std::shared_ptr<arepa::server::Connection>& connection);


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
     * Adds a user to a room.
     *
     * @param user The user that will join the room.
     * @param room The room to join.
     */
    void user_join_room(const std::shared_ptr<User>& user, const std::shared_ptr<Room>& room);

    /**
     * Removes a user from a room.
     * 
     * @param user The user that will leave the room.
     * @param room The room to leave.
     */
    void user_leave_room(const std::shared_ptr<User>& user, const std::shared_ptr<Room>& room);

    /**
     * Sets a user's nickname.
     *
     * @param user The user.
     * @param name The user's new nickname.
     *
     * @throws GameException::USER_NICKNAME_TAKEN If the user's nickname was taken.
     */
    void user_set_nickname(const std::shared_ptr<User>& user, const User::Name& name);

    /**
     * Finds the room that a given user is inside.
     * 
     * @param user The user.
     * @return The room, or nullopt if they aren't in a room.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Room>> find_user_room(const std::shared_ptr<User>& user);

    /**
     * Finds the room that a given user is inside.
     *
     * @param user The user ID.
     * @return The room, or nullopt if they aren't in a room.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Room>> find_user_room(const UserId& user);

    /**
     * Finds the room with a given room ID.
     *
     * @param room The room ID.
     * @return The room, or nullopt if no such room exists.
     */
    [[nodiscard]] std::optional<std::shared_ptr<Room>> find_room(const RoomId& room);

    /**
     * Finds the User from a user ID.
     *
     * @param user The user ID.
     * @return The user object, or nullopt if they don't have an associated object.
     */
    [[nodiscard]] std::optional<std::shared_ptr<User>> find_user(const UserId& user);

    /**
     * Adds a User object to the user map.
     * @param user The user object.
     */
    void add_user(const std::shared_ptr<User>& user);

    /**
     * Removes a User object from the user map.
     * This will also kick them from their room.
     * 
     * @param user The user object.
     */
    void remove_user(const std::shared_ptr<User>& user);
};

// }
