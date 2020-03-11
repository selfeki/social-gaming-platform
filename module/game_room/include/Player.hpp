#pragma once

#include "PlayerNickname.hpp"
#include "PlayerStats.hpp"
#include "PlayerStatus.hpp"

#include <arepa/networking/SessionId.hpp>

#include <optional>

namespace arepa::game::room {

/**
 * A player.
 */
class Player {

#pragma mark - Types -
public:
    using Id = arepa::networking::SessionId;
    using Stats = PlayerStats;
    using Name = PlayerNickname;
    using Status = PlayerStatus;


#pragma mark - Fields -
private:
    Status _status;
    Stats _stats;
    Name _name;
    std::optional<Name> _nickname;
    Id _id;


#pragma mark - Fields (Public) -
public:
    /**
     * The player's stats.
     */
    Stats stats;


#pragma mark - Constructors -
public:
    /**
     * Creates a new player instance.
     * @param id The player's session ID.
     */
    explicit Player(arepa::networking::SessionId id);


#pragma mark - Methods -

    /**
     * Gets the player's ID.
     * @return The player's ID.
     */
    [[nodiscard]] const Id& id() const;

    /**
     * Gets the player's effective name.
     * This is either their nickname (if set), or their UUID.
     *
     * @return The player's name.
     */
    [[nodiscard]] const Name& name() const;

    /**
     * Gets the player's nickname.
     * @return A reference to the player's nickname.
     */
    [[nodiscard]] const std::optional<Name>& nickname() const;

    /**
     * Sets the player's nickname.
     * @param name The player's new nickname.
     */
    void set_nickname(Name name);

    /**
     * Clears the player's nickname.
     */
    void clear_nickname();

    /**
     * Sets the player's status.
     * @param status The player's new status.
     */
    void set_status(Status status);

    /**
     * Checks if the player is a spectator.
     * @return True if the player is a spectator.
     */
    [[nodiscard]] bool is_spectator() const;

    /**
     * Checks if the player is spectating a game.
     * This will always be true if the player is a spectator.
     * 
     * @return True if the player is spectating a game.
     */
    [[nodiscard]] bool is_spectating() const;

    /**
     * Checks if the player is currently playing a game.
     * @return True if the player is playing a game.
     */
    [[nodiscard]] bool is_playing() const;


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const Player& other) const;
    [[nodiscard]] bool operator!=(const Player& other) const;
};

}
