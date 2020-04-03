#include "AbstractUser.hpp"

using namespace arepa::game;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

AbstractUser::AbstractUser(arepa::networking::SessionId id)
    : _name(UserNickname(id.to_string()))
    , _status(Status::UNKNOWN)
    , _id(id) {}

AbstractUser::AbstractUser(arepa::networking::SessionId id, AbstractUser::Name nickname)
    : _name(UserNickname(id.to_string()))
    , _nickname(std::move(nickname))
    , _status(Status::UNKNOWN)
    , _id(id) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (User) -
// ---------------------------------------------------------------------------------------------------------------------

const User::Id& AbstractUser::id() const {
    return this->_id;
}

const User::Name& AbstractUser::name() const {
    return this->_name;
}

const std::optional<User::Name>& AbstractUser::nickname() const {
    return this->_nickname;
}

void AbstractUser::set_nickname(Name name) {
    this->_name = name;
    this->_nickname = std::move(name);
}

void AbstractUser::clear_nickname() {
    this->_name = UserNickname(this->_id.to_string());
    this->_nickname = std::nullopt;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Player) -
// ---------------------------------------------------------------------------------------------------------------------

const Player::Stats& AbstractUser::stats() {
    return this->_stats;
}

const Player::Stats& AbstractUser::stats() const {
    return this->_stats;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool AbstractUser::operator==(const AbstractUser& other) const {
    return this->_id == other._id;
}

bool AbstractUser::operator!=(const AbstractUser& other) const {
    return !(*this == other);
}
