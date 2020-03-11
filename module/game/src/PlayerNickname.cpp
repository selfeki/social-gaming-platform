#include "PlayerNickname.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

using namespace arepa::game;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

PlayerNickname::PlayerNickname(const std::string& name)
    : PlayerNickname(std::string_view(name)) {}

PlayerNickname::PlayerNickname(const char* name)
    : PlayerNickname(std::string_view(name)) {}

PlayerNickname::PlayerNickname(const std::string_view& name) {
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

std::optional<PlayerNickname> PlayerNickname::parse(const std::string& name) {
    return PlayerNickname::parse(std::string_view(name));
}

std::optional<PlayerNickname> PlayerNickname::parse(const std::string_view& name) {
    try {
        return { PlayerNickname(name) };
    } catch (std::runtime_error& ex) {
        return std::nullopt;
    }
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

const std::string& PlayerNickname::operator*() const {
    return this->_name;
}

PlayerNickname::operator std::string() const {
    return this->_name;
}

bool PlayerNickname::operator==(const PlayerNickname& other) const {
    return this->_name == other._name;
}

bool PlayerNickname::operator==(const std::string& other) const {
    return this->_name == other;
}

bool PlayerNickname::operator!=(const PlayerNickname& other) const {
    return !(*this == other);
}

bool PlayerNickname::operator!=(const std::string& other) const {
    return !(*this == other);
}
