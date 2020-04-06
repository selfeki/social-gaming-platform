#pragma once

#include "PlayerStats.hpp"
#include "internal/BaseUser.hpp"

#include <optional>

namespace arepa::chat {

/**
 * An interface representing a spectator.
 */
class Spectator : public virtual BaseUser {

#pragma mark - Types -
public:
    using Id = BaseUser::Id;
    using Name = BaseUser::Name;


#pragma mark - Operators -
public:
    [[nodiscard]] bool operator==(const Spectator& other) const;
    [[nodiscard]] bool operator!=(const Spectator& other) const;
};

}
