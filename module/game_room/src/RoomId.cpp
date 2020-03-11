#include "RoomId.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

using namespace arepa::game::room;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Tables -
// ---------------------------------------------------------------------------------------------------------------------

static const char ALPHANUM[] = "0123456789"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz";

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

RoomId::RoomId(const std::string& id)
    : RoomId(std::string_view(id)) {}

RoomId::RoomId(const std::string_view& id) {
    auto found = std::find_if_not(id.begin(), id.end(), [](auto& character) {
        return std::isalnum(character);
    });

    if (found != id.end()) {
        throw std::runtime_error("Invalid room ID.");
    }

    this->_id = id;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Static) -
// ---------------------------------------------------------------------------------------------------------------------

std::optional<RoomId> RoomId::parse(const std::string& id) {
    return RoomId::parse(std::string_view(id));
}

std::optional<RoomId> RoomId::parse(const std::string_view& id) {
    try {
        return { RoomId(id) };
    } catch (std::runtime_error& ex) {
        return std::nullopt;
    }
}

RoomId RoomId::generate(size_t length) {
    std::stringstream code;

    for (size_t i = 0; i < length; ++i) {
        code << (ALPHANUM[rand() % (sizeof(ALPHANUM) - 1)]);
    }

    return RoomId(code.str());
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

const std::string& RoomId::operator*() const {
    return this->_id;
}

RoomId::operator std::string() const {
    return this->_id;
}

bool RoomId::operator==(const RoomId& other) const {
    return this->_id == other._id;
}

bool RoomId::operator==(const std::string& other) const {
    return this->_id == other;
}

bool RoomId::operator!=(const RoomId& other) const {
    return !(*this == other);
}

bool RoomId::operator!=(const std::string& other) const {
    return !(*this == other);
}
