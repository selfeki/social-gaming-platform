#pragma once

#include "Player.hpp"
#include "Spectator.hpp"
#include "UserCommunication.hpp"
#include "UserNickname.hpp"
#include "internal/BaseUser.hpp"

#include <arepa/networking/SessionId.hpp>

#include <optional>

namespace arepa::chat {
class Room;

/**
 * The common implementation of the Player and Spectator interfaces.
 */
class User : public virtual UserCommunication, public Player, public Spectator {
    friend Room;

#pragma mark - Types -
public:
    using Id = arepa::networking::SessionId;
    using Stats = PlayerStats;
    using Name = UserNickname;


#pragma mark - Fields -
private:
    Name _name;
    std::optional<Name> _nickname;
    Stats _stats;
    Id _id;


#pragma mark - Constructors -
public:
    /**
     * Creates a new abstract user instance.
     * 
     * @param id The user's ID.
     */
    explicit User(arepa::networking::SessionId id);

    /**
     * Creates a new abstract user instance.
     * 
     * @param id The user's ID.
     * @param nickname The user's nickname.
     */
    explicit User(arepa::networking::SessionId id, Name nickname);


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
    [[nodiscard]] bool operator==(const User& other) const;
    [[nodiscard]] bool operator!=(const User& other) const;
};

}
