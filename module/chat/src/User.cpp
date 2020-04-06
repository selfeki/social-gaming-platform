#include "User.hpp"

using namespace arepa::chat;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

User::User(arepa::networking::SessionId id)
    : _name(UserNickname(id.to_string()))
    , _id(id) {}

User::User(arepa::networking::SessionId id, User::Name nickname)
    : _name(UserNickname(id.to_string()))
    , _nickname(std::move(nickname))
    , _id(id) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (User) -
// ---------------------------------------------------------------------------------------------------------------------

const BaseUser::Id& User::id() const {
    return this->_id;
}

const BaseUser::Name& User::name() const {
    return this->_name;
}

const std::optional<BaseUser::Name>& User::nickname() const {
    return this->_nickname;
}

void User::set_nickname(Name name) {
    this->_name = name;
    this->_nickname = std::move(name);
}

void User::clear_nickname() {
    this->_name = UserNickname(this->_id.to_string());
    this->_nickname = std::nullopt;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Player) -
// ---------------------------------------------------------------------------------------------------------------------

const Player::Stats& User::stats() {
    return this->_stats;
}

const Player::Stats& User::stats() const {
    return this->_stats;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool User::operator==(const User& other) const {
    return this->_id == other._id;
}

bool User::operator!=(const User& other) const {
    return !(*this == other);
}
