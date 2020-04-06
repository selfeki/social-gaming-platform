#include "UserNickname.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

using namespace arepa::chat;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

UserNickname::UserNickname(const std::string& name)
    : UserNickname(std::string_view(name)) {}

UserNickname::UserNickname(const char* name)
    : UserNickname(std::string_view(name)) {}

UserNickname::UserNickname(const std::string_view& name) {
    auto found = std::find_if_not(name.begin(), name.end(), [](auto& character) {
        return std::isalnum(character) || character == '_' || character == '-';
    });

    if (found != name.end()) {
        throw std::runtime_error("Invalid player nickname.");
    }

    this->_name = name;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Static) -
// ---------------------------------------------------------------------------------------------------------------------

std::optional<UserNickname> UserNickname::parse(const std::string& name) {
    return UserNickname::parse(std::string_view(name));
}

std::optional<UserNickname> UserNickname::parse(const std::string_view& name) {
    try {
        return {UserNickname(name) };
    } catch (std::runtime_error& ex) {
        return std::nullopt;
    }
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

const std::string& UserNickname::operator*() const {
    return this->_name;
}

UserNickname::operator std::string() const {
    return this->_name;
}

bool UserNickname::operator==(const UserNickname& other) const {
    return this->_name == other._name;
}

bool UserNickname::operator==(const std::string& other) const {
    return this->_name == other;
}

bool UserNickname::operator!=(const UserNickname& other) const {
    return !(*this == other);
}

bool UserNickname::operator!=(const std::string& other) const {
    return !(*this == other);
}
