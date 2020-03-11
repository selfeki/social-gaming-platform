#include "Player.hpp"

using namespace arepa::game;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Player::Player(arepa::networking::SessionId id, std::shared_ptr<PlayerNetworking> networking)
    : _name(PlayerNickname(id.to_string()))
    , _id(id)
    , _io(std::move(networking)) {}


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
    return this->is_spectator() || this->_status == Player::Status::SPECTATOR_WAITLIST;
}

bool Player::is_spectator() const {
    return this->_status == Player::Status::SPECTATOR;
}

void Player::send(const std::string& message) const {
    this->_io->send_message(message);
}

void Player::send_system_message(const std::string& message) const {
    this->_io->send_message("[system] " + message);
}

void Player::send_packet(const PlayerNetworking::Packet& packet) const {
    this->_io->send_packet(packet);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool Player::operator==(const Player& other) const {
    return this->_id == other._id;
}

bool Player::operator==(const Player::Id& other) const {
    return this->_id == other;
}

bool Player::operator!=(const Player& other) const {
    return !(*this == other);
}

bool Player::operator!=(const Player::Id& other) const {
    return !(*this == other);
}
