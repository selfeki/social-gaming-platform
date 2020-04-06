#pragma once

#include "../PlayerStats.hpp"
#include "../UserCommunication.hpp"
#include "../UserNickname.hpp"

#include <arepa/networking/SessionId.hpp>

#include <optional>

namespace arepa::chat {

/**
 * An interface representing a user.
 */
class BaseUser : public virtual UserCommunication {

#pragma mark - Types -
public:
    using Id = arepa::networking::SessionId;
    using Name = UserNickname;


#pragma mark - Methods -
public:
    /**
     * Gets the user's ID.
     * @return The user's ID.
     */
    [[nodiscard]] virtual const Id& id() const = 0;

    /**
     * Gets the user's effective name.
     * This is either their nickname (if set), or their UUID.
     *
     * @return The user's name.
     */
    [[nodiscard]] virtual const Name& name() const = 0;

    /**
     * Gets the user's nickname.
     * @return A reference to the user's nickname.
     */
    [[nodiscard]] virtual const std::optional<Name>& nickname() const = 0;

    /**
     * Sets the user's nickname.
     * @param name The user's new nickname.
     */
    virtual void set_nickname(Name name) = 0;

    /**
     * Clears the user's nickname.
     */
    virtual void clear_nickname() = 0;


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const BaseUser& other) const;
    [[nodiscard]] bool operator==(const BaseUser::Id& other) const;
    [[nodiscard]] bool operator!=(const BaseUser& other) const;
    [[nodiscard]] bool operator!=(const BaseUser::Id& other) const;
};

}
