#pragma once

#include "UserNickname.hpp"
#include "PlayerStats.hpp"
#include "UserStatus.hpp"
#include "interface/UserNetworking.hpp"

#include <arepa/networking/SessionId.hpp>

#include <optional>

namespace arepa::game {
class Room;

/**
 * A user.
 */
class User {
    friend Room;

#pragma mark - Types -
public:
    using Id = arepa::networking::SessionId;
    using Stats = PlayerStats;
    using Name = UserNickname;
    using Status = UserStatus;


#pragma mark - Fields -
private:
    Status _status;
    Stats _stats;
    Name _name;
    std::optional<Name> _nickname;
    Id _id;
    std::shared_ptr<UserNetworking> _io;


#pragma mark - Fields (Public) -
public:
    /**
     * The user's stats.
     */
    Stats stats;


#pragma mark - Constructors -
public:
    /**
     * Creates a new user instance.
     * @param id The user's session ID.
     * @param networking The user's networking interface.
     */
    explicit User(arepa::networking::SessionId id, std::shared_ptr<UserNetworking> networking);


#pragma mark - Methods -

    /**
     * Gets the user's ID.
     * @return The user's ID.
     */
    [[nodiscard]] const Id& id() const;

    /**
     * Gets the user's effective name.
     * This is either their nickname (if set), or their UUID.
     *
     * @return The user's name.
     */
    [[nodiscard]] const Name& name() const;

    /**
     * Gets the user's nickname.
     * @return A reference to the user's nickname.
     */
    [[nodiscard]] const std::optional<Name>& nickname() const;

    /**
     * Sets the user's nickname.
     * @param name The user's new nickname.
     */
    void set_nickname(Name name);

    /**
     * Clears the user's nickname.
     */
    void clear_nickname();

    /**
     * Checks if the user is a spectator.
     * @return True if the user is a spectator.
     */
    [[nodiscard]] bool is_spectator() const;

    /**
     * Checks if the user is spectating a game.
     * This will always be true if the user is a spectator.
     * 
     * @return True if the user is spectating a game.
     */
    [[nodiscard]] bool is_spectating() const;

    /**
     * Checks if the user is currently playing a game.
     * @return True if the user is playing a game.
     */
    [[nodiscard]] bool is_playing() const;

    /**
     * Sends a packet to the user.
     * @param packet The packet.
     */
    void send_packet(const UserNetworking::Packet& packet) const;

    /**
     * Sends a message to the user.
     * This is for notifying the user of system events.
     *
     * @param message The message.
     */
    void send_system_message(const std::string& message) const;

    /**
     * Sends an error message to the user.
     * This is for notifying the user of user errors.
     *
     * @param message The message.
     */
    void send_error_message(const std::string& message) const;

    /**
     * Sends a system message to the user.
     * @param message The message.
     */
    void send(const std::string& message) const;


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const User& other) const;
    [[nodiscard]] bool operator==(const User::Id& other) const;
    [[nodiscard]] bool operator!=(const User& other) const;
    [[nodiscard]] bool operator!=(const User::Id& other) const;
};

}
