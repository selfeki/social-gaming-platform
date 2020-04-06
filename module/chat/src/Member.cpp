#include "Member.hpp"

using namespace arepa::chat;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Static -
// ---------------------------------------------------------------------------------------------------------------------

size_t Member::COUNTER = 0;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------
Member::Member(std::shared_ptr<User> user, Status status)
    : _since(std::chrono::system_clock::now())
    , _user(std::move(user))
    , _unique(Member::COUNTER++)
    , status(status) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

const std::shared_ptr<User>& Member::user() const {
    return this->_user;
}

std::shared_ptr<User>& Member::user() {
    return this->_user;
}

const std::chrono::system_clock::time_point& Member::since() const {
    return this->_since;
}

bool Member::is_player() const {
    return this->status == Status::PLAYER;
}

bool Member::is_spectator() const {
    return this->status == Status::SPECTATOR
        || this->status == Status::PLAYER_WAITLIST;
}

bool Member::is_waitlisted() const {
    return this->status == Status::PLAYER_WAITLIST;
}

bool Member::is_disqualified() const {
    return this->disqualified;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

User& Member::operator*() {
    return *this->_user;
}

const User& Member::operator*() const {
    return *this->_user;
}

User* Member::operator->() {
    return &*this->_user;
}

const User* Member::operator->() const {
    return &*this->_user;
}

bool Member::operator==(const Member& other) const {
    return this->_user == other._user && this->_unique == other._unique;
}

bool Member::operator!=(const Member& other) const {
    return !(*this == other);
}

bool Member::operator<(const Member& other) const {
    return this->_since < other._since;
}

bool Member::operator>(const Member& other) const {
    return this->_since > other._since;
}

bool arepa::chat::operator==(const Member& lhs, const std::shared_ptr<User>& rhs) {
    return lhs.user() == rhs;
}

bool arepa::chat::operator!=(const Member& lhs, const std::shared_ptr<User>& rhs) {
    return !(lhs == rhs);
}

bool arepa::chat::operator==(const std::shared_ptr<User>& lhs, const Member& rhs) {
    return rhs == lhs;
}

bool arepa::chat::operator!=(const std::shared_ptr<User>& lhs, const Member& rhs) {
    return rhs != lhs;
}
