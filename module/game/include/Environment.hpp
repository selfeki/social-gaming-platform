#pragma once

#include <arepa/chat/MemberPtr.hpp>
#include <arepa/chat/Player.hpp>
#include <arepa/chat/Spectator.hpp>
#include <arepa/chat/User.hpp>

#include <memory>
#include <set>

namespace arepa::game {

/**
 * This is an interface that allows a game instance to manipulate the game room.
 * Most of the methods defined here are a subset of those defined in Room.hpp
 */
class Environment {
#pragma mark - Constructors -
public:
    virtual ~Environment() = default;


#pragma mark - Methods -
public:
    /**
     * Flag a player as disqualified.
     * This effectively converts them to a game spectator.
     * 
     * @param player The player to disqualify.
     */
    virtual void disqualify_player(const arepa::chat::MemberPtr<arepa::chat::Player>& player) = 0;

    /**
     * Reset all player flags (e.g. disqualifications).
     */
    virtual void reset_players() = 0;


#pragma mark - Methods (Room) -
public:
    /**
     * Sets the player limit.
     * Excess players will be added as waitlisted players.
     *
     * @param limit The new player limit.
     */
    virtual void set_player_limit(size_t limit) = 0;

    /**
     * Gets the number of players in the room.
     * @return The spectator count.
     */
    [[nodiscard]] virtual size_t player_count() const = 0;

    /**
     * Gets the number of active players (non-disqualified) in the room.
     * @return The active player count.
     */
    [[nodiscard]] virtual size_t active_player_count() const = 0;

    /**
     * Gets the number of spectators in the room.
     * @return The spectator count.
     */
    [[nodiscard]] virtual size_t spectator_count() const = 0;

    /**
     * Gets the maximum number of players in this room.
     * @return The maximum number of players.
     */
    [[nodiscard]] virtual size_t player_limit() const = 0;

    /**
     * Gets the maximum number of spectators in this room.
     * @return The maximum number of players.
     */
    [[nodiscard]] virtual size_t spectator_limit() const = 0;

    /**
     * Gets the players currently playing a game.
     * This may be different than the users in the room (i.e. when a spectator joins an in-progress game).
     *
     * @return The players currently playing a game.
     */
    [[nodiscard]] virtual const std::set<arepa::chat::MemberPtr<arepa::chat::Player>>& players() const = 0;

    /**
     * Gets the spectators in this room.
     * @return The spectators in this room.
     */
    [[nodiscard]] virtual const std::set<arepa::chat::MemberPtr<arepa::chat::Spectator>>& spectators() const = 0;

    /**
     * Gets the room owner.
     * @return The room owner. This will be nullptr when the room is empty.
     */
    [[nodiscard]] virtual arepa::chat::MemberPtr<arepa::chat::User> owner() const = 0;

    /**
     * Finds a spectator in this room.
     *
     * @param player The spectator ID.
     * @return The spectator pointer, or nullopt if not found.
     */
    [[nodiscard]] virtual arepa::chat::MemberPtr<arepa::chat::Spectator> find_spectator(arepa::chat::User::Id spectator) const = 0;

    /**
     * Finds a player in this room.
     *
     * @param player The player ID.
     * @return The player pointer, or nullptr if not found.
     */
    [[nodiscard]] virtual arepa::chat::MemberPtr<arepa::chat::Player> find_player(arepa::chat::User::Id player) const = 0;

    /**
     * Finds a player in this room.
     *
     * @param player The player name.
     * @return The player pointer, or nullopt if not found.
     */
    [[nodiscard]] virtual arepa::chat::MemberPtr<arepa::chat::Player> find_player(arepa::chat::User::Name player) const = 0;

    /**
     * Finds a spectator in this room.
     *
     * @param player The spectator name.
     * @return The spectator pointer, or nullopt if not found.
     */
    [[nodiscard]] virtual arepa::chat::MemberPtr<arepa::chat::Spectator> find_spectator(arepa::chat::User::Name spectator) const = 0;

    /**
     * Checks if the room is in an active game.
     * @return True if a game is active.
     */
    [[nodiscard]] virtual bool is_game_active() const = 0;

    /**
     * Broadcasts a message to all players and spectators in the room.
     * @param message The message to broadcast.
     */
    virtual void broadcast_message(const std::string& message) = 0;

    /**
     * Broadcasts a message to all players and spectators in the room.
     * @param message The message to broadcast.
     */
    virtual void broadcast_message_to_spectators(const std::string& message) = 0;

    /**
     * Broadcasts a packet to all players and spectators in the room.
     * @param packet The packet to broadcast.
     */
    virtual void broadcast_packet(const arepa::chat::User::Packet& packet) = 0;

    /**
     * Broadcasts a packet to all spectators in the room.
     * @param message The message to broadcast.
     */
    virtual void broadcast_packet_to_spectators(const arepa::chat::User::Packet& packet) = 0;

    /**
     * Starts or restarts a game in this room.
     */
    virtual void start_game() = 0;

    /**
     * Ends the game in this room.
     */
    virtual void end_game() = 0;
};

}
