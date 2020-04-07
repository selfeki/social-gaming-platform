#pragma once

#include "User.hpp"

#include <chrono>

namespace arepa::chat {
class Room;

/**
 * A room member.
 */
class Member {
    friend Room;

#pragma mark - Types -
private:
    using User = arepa::chat::User;

    /**
     * The member's status.
     */
    enum class Status {

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


#pragma mark - Static -
private:
    static size_t COUNTER;


#pragma mark - Field -
private:
    std::chrono::system_clock::time_point _since;
    std::shared_ptr<User> _user;
    size_t _unique;    // Used to make sure members of different rooms don't compare.
    Status status;

    bool disqualified = false;
    bool suppress_broadcasts = false;


#pragma mark - Constructors -
private:
    Member(std::shared_ptr<User> user, Status status);


#pragma mark - Methods -
public:
    /**
     * Gets the user that this refers to.
     * @return The user.
     */
    [[nodiscard]] const std::shared_ptr<User>& user() const;

    /**
     * Gets the user that this refers to.
     * @return The user.
     */
    [[nodiscard]] std::shared_ptr<User>& user();

    /**
     * Gets the time at which the user joined the room.
     * @return The user's join time.
     */
    const std::chrono::system_clock::time_point& since() const;

    /**
     * Checks if the user is a spectator or on the waitlist.
     * @return True if the user is a spectator.
     */
    bool is_spectator() const;

    /**
     * Checks if the user is on the waitlist.
     * @return True if the user is on the waitlist.
     */
    bool is_waitlisted() const;

    /**
     * Checks if the user is a player who lost.
     * @return True if the user lost the current game.
     */
    bool is_disqualified() const;

    /**
     * Checks if the user is a current (or disqualified) player.
     * @return True if the user is a player.
     */
    bool is_player() const;


#pragma mark - Operators -
public:
    User& operator*();
    const User& operator*() const;
    User* operator->();
    const User* operator->() const;
    bool operator==(const Member& other) const;
    bool operator!=(const Member& other) const;
    bool operator<(const Member& other) const;
    bool operator>(const Member& other) const;
};

#pragma mark - Operators -
bool operator==(const Member& lhs, const std::shared_ptr<User>& rhs);
bool operator!=(const Member& lhs, const std::shared_ptr<User>& rhs);
bool operator==(const std::shared_ptr<User>& lhs, const Member& rhs);
bool operator!=(const std::shared_ptr<User>& lhs, const Member& rhs);

}
