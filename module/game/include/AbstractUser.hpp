#pragma once

#include "Player.hpp"
#include "Spectator.hpp"
#include "User.hpp"
#include "UserNickname.hpp"
#include "UserStatus.hpp"

#include <arepa/networking/SessionId.hpp>

#include <optional>

namespace arepa::game {
class Room;

/**
 * The common implementation of the User, Player, and Spectator interfaces.
 */
class AbstractUser : public virtual UserCommunication, public virtual User, public Player, public Spectator {
    friend Room;

#pragma mark - Types -
public:
    using Id = arepa::networking::SessionId;
    using Stats = PlayerStats;
    using Name = UserNickname;
    using Status = UserStatus;


#pragma mark - Fields -
private:
    Name _name;
    std::optional<Name> _nickname;
    Status _status;
    Stats _stats;
    Id _id;


#pragma mark - Constructors -
public:
    /**
     * Creates a new abstract user instance.
     * 
     * @param id The user's ID.
     */
    explicit AbstractUser(arepa::networking::SessionId id);

    /**
     * Creates a new abstract user instance.
     * 
     * @param id The user's ID.
     * @param nickname The user's nickname.
     */
    explicit AbstractUser(arepa::networking::SessionId id, Name nickname);


#pragma mark - Methods (User) -
public:
    [[nodiscard]] const Id& id() const override;
    [[nodiscard]] const Name& name() const override;
    [[nodiscard]] const std::optional<Name>& nickname() const override;
    void set_nickname(Name name) override;
    void clear_nickname() override;


#pragma mark - Methods (Player) -
public:
    const Player::Stats& stats() const override;
    const Player::Stats& stats() override;


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const AbstractUser& other) const;
    [[nodiscard]] bool operator!=(const AbstractUser& other) const;
};

}
