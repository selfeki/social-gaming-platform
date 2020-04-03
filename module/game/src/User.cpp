#include "User.hpp"

using namespace arepa::game;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

User::User(arepa::networking::SessionId id, std::shared_ptr<UserNetworking> networking)
    : _name(UserNickname(id.to_string()))
    , _id(id)
    , _io(std::move(networking)) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

const User::Id& User::id() const {
    return this->_id;
}

const User::Name& User::name() const {
    return this->_name;
}

const std::optional<User::Name>& User::nickname() const {
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

bool User::is_playing() const {
    return this->_status == User::Status::PLAYING;
}

bool User::is_spectating() const {
    return this->is_spectator() || this->_status == User::Status::SPECTATOR_WAITLIST;
}

bool User::is_spectator() const {
    return this->_status == User::Status::SPECTATOR;
}

void User::send(const std::string& message) const {
    this->_io->send_message(message);
}

void User::send_system_message(const std::string& message) const {
    this->_io->send_message(message);
}

void User::send_error_message(const std::string& message) const {
    this->_io->send_error_message(message);
}

void User::send_packet(const UserNetworking::Packet& packet) const {
    this->_io->send_packet(packet);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool User::operator==(const User& other) const {
    return this->_id == other._id;
}

bool User::operator==(const User::Id& other) const {
    return this->_id == other;
}

bool User::operator!=(const User& other) const {
    return !(*this == other);
}

bool User::operator!=(const User::Id& other) const {
    return !(*this == other);
}
