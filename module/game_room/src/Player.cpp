#include "Player.hpp"

using namespace arepa::game::room;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Player::Player(arepa::networking::SessionId id)
    : _name(PlayerNickname(id.to_string()))
    , _id(id) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

const Player::Id& Player::id() const {
    return this->_id;
}

const Player::Name& Player::name() const {
    return this->_name;
}

const std::optional<Player::Name>& Player::nickname() const {
    return this->_nickname;
}

void Player::set_nickname(Name name) {
    this->_name = name;
    this->_nickname = std::move(name);
}

void Player::clear_nickname() {
    this->_name = PlayerNickname(this->_id.to_string());
    this->_nickname = std::nullopt;
}

bool Player::is_playing() const {
    return this->_status == Player::Status::PLAYING;
}

bool Player::is_spectating() const {
    return this->is_spectator() || this->_status == Player::Status::WAITLIST;
}

bool Player::is_spectator() const {
    return this->_status == Player::Status::SPECTATOR;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool Player::operator==(const Player& other) const {
    return this->_id == other._id;
}

bool Player::operator!=(const Player& other) const {
    return !(*this == other);
}
