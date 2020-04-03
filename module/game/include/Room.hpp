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
 * This is a room that allows users to chat and play games together.
 *
 * Terminology:
 * - "user"      -- A human.
 * - "player"    -- A user who is currently in a game, or waiting to start a game.
 * - "spectator" -- A user who is watching games.
 */
class Room {
#pragma mark - Types -
public:
    using Id = RoomId;
    using User = AbstractUser;
    using Player = arepa::game::Player;
    using Spectator = arepa::game::Spectator;
    using CommandMap = arepa::command::CommandMap<AbstractUser&, Room&>;
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

#pragma mark - Constants -
public:
    /**
     * Join the room as a player, or as a spectator if that's not possible.
     *
     * If the room is full of players and spectators, an exception will be thrown.
     */
    enum class Join {

        /**
          * Join the room as a player.
          *
          * This is not possible when:
          * - The room is full of players.
          * - The room is currently playing a game.
          */
        AS_PLAYER,

        /**
          * Join the room as a player waiting to join the game.
          *
          * This is not possible when:
          * - The room is full of players and spectators.
          */
        AS_PLAYER_WAITING,

        /**
          * Join the room as a spectator.
          *
          * This is not possible when:
          * - The room is full of spectators.
          * - The room is currently playing a game.
          */
        AS_SPECTATOR

    };


private:
    enum class UserStatus {

        /**
         * The user is an in-game player.
         */
        PLAYER,

        /**
         * The user is in the waitlist.
         */
        PLAYER_WAITLIST,

        /**
         * The user is a spectator.
         */
        SPECTATOR

    };

    struct UserContainer {
        std::chrono::system_clock::time_point joined_at;
        UserStatus status;
        std::shared_ptr<User> user;

        UserContainer(std::shared_ptr<User> user, UserStatus status);
    };

    /**
     * A guard that allows for changing the user container without leading to inconsistent state.
     */
    class UserContainerGuard {
    private:
        Room& _room;
        UserContainer& _container;

    public:
        UserContainerGuard(Room* room, UserContainer& container);
        ~UserContainerGuard();
    };


#pragma mark - Signals -
private:
    //    arepa::communication::Signal<std::shared_ptr<User>&> on_user_join;
    //    arepa::communication::Signal<std::shared_ptr<User>&> on_user_leave;
    //    arepa::communication::Signal<std::shared_ptr<User>&> on_user_is_spectating;
    //    arepa::communication::Signal<std::shared_ptr<User>&> on_user_is_playing;
    //    arepa::communication::Signal<> on_game_start;
    //    arepa::communication::Signal<> on_game_end;


#pragma mark - Fields -
private:
    std::shared_ptr<User> _owner;
    std::chrono::system_clock::time_point _created_at;
    Id _id;

    size_t _player_limit = 8;
    size_t _spectator_limit = 12;
    std::map<User::Id, UserContainer> _users;
    std::set<std::shared_ptr<User>> _cached_users;
    std::set<std::shared_ptr<Player>> _cached_players;
    std::set<std::shared_ptr<Spectator>> _cached_spectators;

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
    explicit Room(Id id);

    /**
     * Creates a new game room with a random room ID.
     * The first player added to this room will be set as the owner.
     */
    Room();


#pragma mark - Methods (Private) -
private:
    /**
     * Finds the user container object from the user map.
     * @param id The user's ID.
     * @return A pointer to the user container, or nullptr if not found.
     */
    [[nodiscard]] const UserContainer* _find_user_container(const User::Id& id) const;

    /**
     * Finds the user container object from the user map.
     * @param id The user's ID.
     * @return A pointer to the user container, or nullptr if not found.
     */
    [[nodiscard]] UserContainer* _find_user_container(const User::Id& id);

    /**
     * Finds the user container object from the user map.
     * @param id The user's name.
     * @return A pointer to the user container, or nullptr if not found.
     */
    [[nodiscard]] const UserContainer* _find_user_container(const User::Name& name) const;

    /**
     * Finds the user container object from the user map.
     * @param id The user's name.
     * @return A pointer to the user container, or nullptr if not found.
     */
    [[nodiscard]] UserContainer* _find_user_container(const User::Name& name);

    /**
     * Finds the user container object from the user map.
     * @param id The user's pointer.
     * @return A pointer to the user container, or nullptr if not found.
     */
    [[nodiscard]] const UserContainer* _find_user_container(const std::shared_ptr<game::User>& user) const;

    /**
     * Finds the user container object from the user map.
     * @param id The user's pointer.
     * @return A pointer to the user container, or nullptr if not found.
     */
    [[nodiscard]] UserContainer* _find_user_container(const std::shared_ptr<game::User>& user);


    /**
     * Checks if the user is a player in the room.
     *
     * @param user The user container.
     * @return True if the user is a player.
     */
    [[nodiscard]] bool is_player(const UserContainer& user) const;

    /**
     * Checks if the user is a spectator in the room.
     *
     * @param user The user container.
     * @return True if the user is a spectator.
     */
    [[nodiscard]] bool is_spectator(const UserContainer& user) const;

    /**
     * Checks if the user is a spectating the current game.
     * Users are only considered to be spectating if they are a spectator, AND a game is running.
     *
     * @param user The user container.
     * @return True if the user is spectating the game.
     */
    [[nodiscard]] bool is_spectating(const UserContainer& user) const;

    /**
     * Checks if the spectator is in the waitlist to become a player.
     *
     * @param spectator The user container.
     * @return True if the spectator is in the waitlist.
     */
    [[nodiscard]] bool is_waitlisted(const UserContainer& user) const;

    /**
     * Calculates a vector of users in the room, sorted by the time they joined.
     *
     * std::less = Newest first.
     * std::greater = Oldest first.
     *
     * @tparam The comparison function.
     * @return A vector of age-sorted users.
     */
    template <typename _Cmp = std::less<std::chrono::system_clock::time_point>>
    [[nodiscard]] std::vector<UserContainer*> _users_by_age() const {
        return this->_users_by_age<_Cmp>([](auto&) { return true; });
    }

    /**
     * Calculates a vector of users in the room, sorted by the time they joined.
     *
     * std::less = Newest first.
     * std::greater = Oldest first.
     *
     * @param filter A filter predicate.
     * @param limit The maximum number of users to return.
     * @tparam The comparison function.
     * @return A vector of age-sorted users.
     */
    template <typename _Cmp = std::less<std::chrono::system_clock::time_point>>
    [[nodiscard]] std::vector<Room::UserContainer*> _users_by_age(size_t limit, const std::function<bool(const UserContainer&)>& filter) const {
        auto users = this->_users_by_age(filter);

        // Trim to limit.
        if (users.size() > limit) {
            users.erase(users.begin() += limit, users.end());
        }

        // Return trimmed.
        return users;
    }

    /**
     * Calculates a vector of users in the room, sorted by the time they joined.
     *
     * std::less = Newest first.
     * std::greater = Oldest first.
     *
     * @param filter A filter predicate.
     * @tparam The comparison function.
     * @return A vector of age-sorted users.
     */
    template <typename _Cmp = std::less<std::chrono::system_clock::time_point>>
    [[nodiscard]] std::vector<Room::UserContainer*> _users_by_age(const std::function<bool(const UserContainer&)>& filter) const {
        std::vector<UserContainer*> users;
        for (auto& pair : this->_users) {
            if (filter(pair.second)) {
                users.push_back(const_cast<UserContainer*>(&pair.second));
            }
        }

        // Sort by time.
        std::sort(users.begin(), users.end(), [this](const auto& a, const auto& b) {
            return _Cmp()(a->joined_at, b->joined_at);
        });

        // Return.
        return users;
    }

    /**
     * Removes the container's user from its associated cached vector.
     * This will remove the user from the cached player or spectator vectors, depending on their status.
     *
     * @param container The user container.
     */
    void _cached_remove(const UserContainer& container);

    /**
     * Adds the container's user to its associated cached vector.
     * This will add the user to the cached player or spectator vectors, depending on their status.
     *
     * @param container The user container.
     */
    void _cached_add(const UserContainer& container);


#pragma mark - Methods -
public:
    /**
     * Gets the room ID.
     * @return The room ID.
     */
    [[nodiscard]] const Id& id() const;

    /**
     * Gets the number of users in the room.
     * @return The user count.
     */
    [[nodiscard]] size_t user_count() const;

    /**
     * Gets the number of players in the room.
     * @return The spectator count.
     */
    [[nodiscard]] size_t player_count() const;

    /**
     * Gets the number of spectators in the room.
     * @return The spectator count.
     */
    [[nodiscard]] size_t spectator_count() const;

    /**
     * Gets the maximum number of players in this room.
     * @return The maximum number of players.
     */
    [[nodiscard]] size_t player_limit() const;

    /**
     * Gets the maximum number of spectators in this room.
     * @return The maximum number of players.
     */
    [[nodiscard]] size_t spectator_limit() const;

    /**
     * Gets the users in this room.
     * @return The users in this room.
     */
    [[nodiscard]] const std::set<std::shared_ptr<User>>& users() const;

    /**
     * Gets the users in this room, sorted by when they joined.
     * @return A sorted vector of users.
     */
    template <typename _Cmp = std::less<std::chrono::system_clock::time_point>>
    [[nodiscard]] const std::vector<std::shared_ptr<User>> users_by_age() const {
        std::vector<std::shared_ptr<User>> users;
        auto sorted = this->_users_by_age();

        std::transform(sorted.begin(), sorted.end(), std::back_inserter(users), [](auto container) {
            return container->user;
        });

        return users;
    }

    /**
     * Gets the players currently playing a game.
     * This may be different than the users in the room (i.e. when a spectator joins an in-progress game).
     *
     * @return The players currently playing a game.
     */
    [[nodiscard]] const std::set<std::shared_ptr<Player>>& players() const;

    /**
     * Gets the spectators in this room.
     * @return The spectators in this room.
     */
    [[nodiscard]] const std::set<std::shared_ptr<Spectator>>& spectators() const;

    /**
     * Gets the date when the room was created.
     * @return The room creation date.
     */
    [[nodiscard]] const std::chrono::system_clock::time_point& creation_date() const;

    /**
     * Checks if the room is in an active game.
     * @return True if a game is active.
     */
    [[nodiscard]] bool is_game_active() const;

    /**
     * Sets the soft player limit.
     * Excess players will be added as waitlisted players.
     *
     * This can be overridden by the game instance.
     *
     * @param limit The new player limit.
     */
    void set_player_limit(size_t limit);

    /**
     * Sets the spectator limit.
     * Excess spectators will be kicked (spectators, then waitlisted players).
     *
     * @param limit The new spectator limit.
     */
    void set_spectator_limit(size_t limit);

    /**
     * Removes a user from the room.
     * @param user The user to remove.
     */
    void remove_user(const std::shared_ptr<User>& user);

    /**
     * Adds a user to the room.
     * 
     * @param user The user to add.
     * @param method The join method.
     */
    void add_user(const std::shared_ptr<User>& user, Join method = Join::AS_PLAYER_WAITING);

    /**
     * Gets the room owner.
     * @return The room owner. This will be nullptr when the room is empty.
     */
    [[nodiscard]] std::shared_ptr<User> owner() const;

    /**
     * Checks if the user is a player in the room.
     * 
     * @param user The user pointer.
     * @return True if the user is a player.
     */
    [[nodiscard]] bool is_player(const std::shared_ptr<User>& user) const;

    /**
     * Checks if the user is a spectator in the room.
     *
     * @param user The user pointer.
     * @return True if the user is a spectator.
     */
    [[nodiscard]] bool is_spectator(const std::shared_ptr<User>& user) const;

    /**
     * Checks if the user is a spectating the current game.
     * Users are only considered to be spectating if they are a spectator, AND a game is running.
     *
     * @param user The user pointer.
     * @return True if the user is spectating the game.
     */
    [[nodiscard]] bool is_spectating(const std::shared_ptr<User>& user) const;

    /**
     * Checks if the spectator is in the waitlist to become a player.
     *
     * @param spectator The user pointer.
     * @return True if the user is in the waitlist.
     */
    [[nodiscard]] bool is_waitlisted(const std::shared_ptr<User>& spectator) const;

    /**
     * Finds a player in this room.
     *
     * @param player The user pointer.
     * @return The player pointer, or nullptr if not found.
     */
    [[nodiscard]] std::shared_ptr<Player> find_player(const std::shared_ptr<User>& player) const;

    /**
     * Finds a spectator in this room.
     *
     * @param player The spectator ID.
     * @return The spectator pointer, or nullopt if not found.
     */
    [[nodiscard]] std::shared_ptr<Spectator> find_spectator(User::Id spectator) const;

    /**
     * Finds a player in this room.
     *
     * @param player The player ID.
     * @return The player pointer, or nullptr if not found.
     */
    [[nodiscard]] std::shared_ptr<Player> find_player(User::Id player) const;

    /**
     * Finds a spectator in this room.
     *
     * @param spectator The user pointer.
     * @return The spectator pointer, or nullptr if not found.
     */
    [[nodiscard]] std::shared_ptr<Spectator> find_spectator(const std::shared_ptr<User>& spectator) const;

    /**
     * Finds a user (player or spectator) in this room.
     *
     * @param player The user ID.
     * @return The user pointer, or nullopt if not found.
     */
    [[nodiscard]] std::shared_ptr<User> find_user(User::Id user) const;

    /**
     * Finds a player in this room.
     *
     * @param player The player name.
     * @return The player pointer, or nullopt if not found.
     */
    [[nodiscard]] std::shared_ptr<Player> find_player(User::Name player) const;

    /**
     * Finds a spectator in this room.
     *
     * @param player The spectator name.
     * @return The spectator pointer, or nullopt if not found.
     */
    [[nodiscard]] std::shared_ptr<Spectator> find_spectator(User::Name spectator) const;

    /**
     * Finds a user (player or spectator) in this room.
     *
     * @param player The user name.
     * @return The user pointer, or nullopt if not found.
     */
    [[nodiscard]] std::shared_ptr<User> find_user(User::Name spectator) const;

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

    /**
     * Broadcasts a message to all players and spectator in the room.
     * @param message The message to broadcast.
     */
    void broadcast_message_to_spectators(const std::string& message);

    /**
     * Broadcasts a packet to all players and spectators in the room.
     * @param packet The packet to broadcast.
     */
    void broadcast_packet(const User::Packet& packet);

    /**
     * Sets the room owner.
     * @throws GameException::PLAYER_NOT_FOUND
     * @param owner The new owner.
     */
    void set_owner(std::shared_ptr<AbstractUser> owner);

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
    bool process_message(AbstractUser::Id player, const std::string& message);

    /**
     * Processes a player or spectator's command.
     *
     * @param player The player or spectator.
     * @param command  The command sent by the player or spectator.
     *
     * @return True if the command was handled by the room.
     */
    bool process_command(AbstractUser::Id player, const arepa::command::Command& command);


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const Room& other) const;
    [[nodiscard]] bool operator!=(const Room& other) const;
};

}
