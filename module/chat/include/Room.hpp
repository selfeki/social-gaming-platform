#pragma once

#include "ChatException.hpp"
#include "Member.hpp"
#include "MemberPtr.hpp"
#include "Player.hpp"
#include "RoomId.hpp"
#include "Spectator.hpp"
#include "User.hpp"

#include <arepa/command/Command.hpp>
#include <arepa/command/CommandMap.hpp>
#include <arepa/command/Executor.hpp>
#include <arepa/communication/Signal.hpp>
#include <arepa/game/Controller.hpp>
#include <arepa/game/Environment.hpp>

#include <chrono>
#include <map>
#include <memory>
#include <set>

namespace arepa::chat {

/**
 * A game room.
 * This is a room that allows users to chat and play games together.
 *
 * Terminology:
 * - "user"      -- A human.
 * - "player"    -- A user who is currently in a game, or waiting to start a game.
 * - "spectator" -- A user who is watching games.
 */
class Room : public arepa::game::Environment {

#pragma mark - Types -
public:
    using Id = RoomId;
    using User = arepa::chat::Member::User;
    using Player = arepa::chat::Player;
    using Spectator = arepa::chat::Spectator;
    using CommandMap = arepa::command::CommandMap<User&, Room&>;
    using CommandExecutor = CommandMap::Executor;

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
    /**
     * A guard that allows for changing the member data without leaving an inconsistent state.
     */
    class MemberGuard {
    private:
        Room& _room;
        Member& _member;

    public:
        MemberGuard(Room* room, Member& member);
        ~MemberGuard();
    };


#pragma mark - Signals -
public:
    arepa::communication::Signal<MemberPtr<User>> on_owner_change;
    arepa::communication::Signal<MemberPtr<User>> on_member_join;
    arepa::communication::Signal<MemberPtr<User>> on_member_leave;


#pragma mark - Fields -
private:
    MemberPtr<User> _owner;
    std::chrono::system_clock::time_point _created_at;
    Id _id;

    size_t _player_limit = 8;
    size_t _spectator_limit = 12;
    size_t _players_disqualified = 0;
    std::map<User::Id, Member> _members;
    std::set<MemberPtr<User>> _cached_users;
    std::set<MemberPtr<Player>> _cached_players;
    std::set<MemberPtr<Spectator>> _cached_spectators;
    std::set<MemberPtr<Spectator>> _cached_users_as_spectators;

    std::unique_ptr<arepa::game::Controller> _game;
    bool _game_started;


#pragma mark - Fields (Public) -
public:
    CommandMap commands;
    CommandMap protected_commands;


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

    ~Room() = default;


#pragma mark - Methods (Private) -
private:
    /**
     * Finds the member object from the user map.
     * @param id The user's ID.
     * @return A pointer to the user member, or nullptr if not found.
     */
    [[nodiscard]] const Member* _find_member(const User::Id& id) const;

    /**
     * Finds the member object from the user map.
     * @param id The user's ID.
     * @return A pointer to the user member, or nullptr if not found.
     */
    [[nodiscard]] inline Member* _find_member(const User::Id& id) {
        return const_cast<Member*>(std::as_const(*this)._find_member(id));
    }

    /**
     * Finds the member object from the user map.
     * @param id The user's name.
     * @return A pointer to the user member, or nullptr if not found.
     */
    [[nodiscard]] const Member* _find_member(const User::Name& name) const;

    /**
     * Finds the member object from the user map.
     * @param id The user's name.
     * @return A pointer to the user member, or nullptr if not found.
     */
    [[nodiscard]] inline Member* _find_member(const User::Name& name) {
        return const_cast<Member*>(std::as_const(*this)._find_member(name));
    }

    /**
     * Finds the member object from the user map.
     * @param id The user's pointer.
     * @return A pointer to the user member, or nullptr if not found.
     */
    [[nodiscard]] const Member* _find_member(const std::shared_ptr<chat::User>& user) const;

    /**
     * Finds the member object from the user map.
     * @param id The user's pointer.
     * @return A pointer to the user member, or nullptr if not found.
     */
    [[nodiscard]] inline Member* _find_member(const std::shared_ptr<chat::User>& user) {
        return const_cast<Member*>(std::as_const(*this)._find_member(user));
    }

    /**
     * Removes a user from the room.
     * @param user The user to remove.
     */
    void _remove_user(const std::shared_ptr<User>& user);

    /**
     * Removes the member reference from its associated cached vector.
     * This will remove the member from the cached player or spectator vectors, depending on their status.
     *
     * @param member The member.
     */
    void _cached_remove(const Member& member);

    /**
     * Adds the member reference to its associated cached vector.
     * This will add the member to the cached player or spectator vectors, depending on their status.
     *
     * @param member The member.
     */
    void _cached_add(Member& member);


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
     * This counts disqualified players in the total.
     * 
     * @return The player count.
     */
    [[nodiscard]] size_t player_count() const final;

    /**
     * Gets the number of active players (non-disqualified) in the room.
     * @return The active player count.
     */
    [[nodiscard]] size_t active_player_count() const final;

    /**
     * Gets the number of spectators in the room.
     * @return The spectator count.
     */
    [[nodiscard]] size_t spectator_count() const final;

    /**
     * Gets the maximum number of players in this room.
     * @return The maximum number of players.
     */
    [[nodiscard]] size_t player_limit() const final;

    /**
     * Gets the maximum number of spectators in this room.
     * @return The maximum number of players.
     */
    [[nodiscard]] size_t spectator_limit() const final;

    /**
     * Gets the users in this room.
     * @return The users in this room.
     */
    [[nodiscard]] const std::set<MemberPtr<User>>& users() const;

    /**
     * Gets the players currently playing a game.
     *
     * When a player is disqualified (i.e. they lost), they will be temporarily removed from this set.
     * The only legitimate way to check a member's room status is to use the helper functions provided by MemberPtr.
     *
     * @return The players currently playing a game.
     */
    [[nodiscard]] const std::set<MemberPtr<Player>>& players() const final;

    /**
     * Gets the spectators in this room.
     * When not playing a game, every player is included in this set.
     *
     * @return The spectators in this room.
     */
    [[nodiscard]] const std::set<MemberPtr<Spectator>>& spectators() const final;

    /**
     * Gets the date when the room was created.
     * @return The room creation date.
     */
    [[nodiscard]] const std::chrono::system_clock::time_point& creation_date() const;

    /**
     * Checks if the room is in an active game.
     * @return True if a game is active.
     */
    [[nodiscard]] bool is_game_active() const final;

    /**
     * Checks if the room has a loaded game.
     * @return True if a game is loaded.
     */
    [[nodiscard]] bool is_game_loaded() const;

    /**
     * Sets the player limit.
     * Excess players will be added as waitlisted players.
     *
     * @param limit The new player limit.
     */
    void set_player_limit(size_t limit) final;

    /**
     * Sets the spectator limit.
     * Excess spectators will be kicked (spectators, then waitlisted players).
     *
     * @param limit The new spectator limit.
     */
    void set_spectator_limit(size_t limit);

    /**
     * Attempt to flush users out of the waitlist and into the player slots.
     * This will do nothing during a game.
     */
    void flush_waitlist();

    /**
     * Removes a user from the room.
     * @param user The user to remove.
     */
    template <typename T>
    void remove_user(const MemberPtr<T>& user) {
        this->_remove_user(user.user());
    }

    /**
     * Adds as user to the room.
     * 
     * @param user The user to add.
     * @param method The join method.
     */
    void add_user(const std::shared_ptr<User>& user, Join method = Join::AS_PLAYER_WAITING);

    /**
     * Gets the room owner.
     * @return The room owner. This will be nullptr when the room is empty.
     */
    [[nodiscard]] MemberPtr<User> owner() const final;

    /**
     * Finds a player in this room.
     *
     * @param player The user pointer.
     * @return The player pointer, or nullptr if not found.
     */
    [[nodiscard]] MemberPtr<Player> find_player(const std::shared_ptr<User>& player) const;

    /**
     * Finds a spectator in this room.
     *
     * @param spectator The user pointer.
     * @return The spectator pointer, or nullptr if not found.
     */
    [[nodiscard]] MemberPtr<Spectator> find_spectator(const std::shared_ptr<User>& spectator) const;

    /**
     * Finds a user (player or spectator) in this room from their non-owned user pointer.
     * A non-owned user pointer is the normal shared_ptr, while the owned user pointer is a
     * MemberPtr that is uniquely tied to the instance of that specific user within this specific room.
     *
     * @param user The non-owned user pointer.
     * @return The user pointer, or nullptr if not found.
     */
    [[nodiscard]] MemberPtr<User> find_user(const std::shared_ptr<User>& user) const;

    /**
     * Finds a spectator in this room.
     *
     * @param player The spectator ID.
     * @return The spectator pointer, or nullopt if not found.
     */
    [[nodiscard]] MemberPtr<Spectator> find_spectator(User::Id spectator) const final;

    /**
     * Finds a player in this room.
     *
     * @param player The player ID.
     * @return The player pointer, or nullptr if not found.
     */
    [[nodiscard]] MemberPtr<Player> find_player(User::Id player) const final;

    /**
     * Finds a user (player or spectator) in this room.
     *
     * @param player The user ID.
     * @return The user pointer, or nullopt if not found.
     */
    [[nodiscard]] MemberPtr<User> find_user(User::Id user) const;

    /**
     * Finds a player in this room.
     *
     * @param player The player name.
     * @return The player pointer, or nullopt if not found.
     */
    [[nodiscard]] MemberPtr<Player> find_player(User::Name player) const final;

    /**
     * Finds a spectator in this room.
     *
     * @param player The spectator name.
     * @return The spectator pointer, or nullopt if not found.
     */
    [[nodiscard]] MemberPtr<Spectator> find_spectator(User::Name spectator) const final;

    /**
     * Finds a user (player or spectator) in this room.
     *
     * @param name The user name.
     * @return The user pointer, or nullopt if not found.
     */
    [[nodiscard]] MemberPtr<User> find_user(User::Name name) const;

    /**
     * Checks if the room is full.
     * @return True if the room is full.
     */
    [[nodiscard]] bool is_full() const;

    /**
     * Broadcasts a message to all players and spectators in the room.
     * @param message The message to broadcast.
     */
    void broadcast_message(const std::string& message) final;

    /**
     * Broadcasts a message to all players and spectators in the room.
     * @param message The message to broadcast.
     */
    void broadcast_message_to_spectators(const std::string& message) final;

    /**
     * Broadcasts a packet to all players and spectators in the room.
     * @param packet The packet to broadcast.
     */
    void broadcast_packet(const User::Packet& packet) final;

    /**
     * Broadcasts a packet to all spectators in the room.
     * @param message The message to broadcast.
     */
    void broadcast_packet_to_spectators(const User::Packet& packet) final;

    /**
     * Sets the room owner.
     * @throws GameException::PLAYER_NOT_FOUND
     * @param owner The new owner.
     */
    template <typename T>
    void set_owner(MemberPtr<T> owner) {
        if (owner && !this->_find_member(owner->id())) {
            throw ChatException(ChatException::USER_NOT_FOUND);
        }

        this->_owner = MemberPtr<User>(owner._ptr);
        if (owner) {
            this->on_owner_change(MemberPtr<User>(owner));
        }
    }

    /**
     * Runs a tick on the game instance.
     */
    void update_game();

    /**
     * Loads a game in this room.
     * @param game The game controller.
     */
    void load_game(std::unique_ptr<arepa::game::Controller> game);

    /**
     * Starts or restarts a game in this room.
     */
    void start_game() final;

    /**
     * Ends the game in this room.
     */
    void end_game() final;

    /**
     * Get the game controller.
     * This will throw if there isn't a game.
     *
     * @return The game controller.
     */
    arepa::game::Controller& game() const;

    /**
     * Set a game option.
     *
     * @param option The option name.
     * @param value The option value.
     * 
     * @return The Result.
     */
    arepa::Result<void, std::string> game_option(const arepa::game::Controller::OptionKey& option, const arepa::game::Controller::OptionValue& value);

    /**
     * Processes a player or spectators's message.
     *
     * @param player  The player or spectator.
     * @param message The message sent by the player or spectator.
     *
     * @return True if the message was handled by the room.
     */
    bool process_message(User::Id player, const std::string& message);

    /**
     * Processes a player or spectator's command.
     *
     * @param player The player or spectator.
     * @param command  The command sent by the player or spectator.
     *
     * @return True if the command was handled by the room.
     */
    bool process_command(User::Id player, const arepa::command::Command& command);


#pragma mark - Methods (Environment) -
public:
    /**
     * Flag a player as disqualified.
     * This effectively converts them to a game spectator.
     *
     * @param player The player to disqualify.
     */
    virtual void disqualify_player(const arepa::chat::MemberPtr<arepa::chat::Player>& player) override;

    /**
     * Reset all player flags (e.g. disqualifications).
     */
    virtual void reset_players() override;


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const Room& other) const;
    [[nodiscard]] bool operator!=(const Room& other) const;
};
}
