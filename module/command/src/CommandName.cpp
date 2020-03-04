#include "CommandName.hpp"

#include <cctype>
#include <stdexcept>
#include <algorithm>

using namespace arepa::command;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

CommandName::CommandName(const std::string& name)
    : CommandName(std::string_view(name)) {}

CommandName::CommandName(const std::string_view& name) {

    auto found = std::find_if_not(name.begin(), name.end(), [](auto & character) {
            return std::isalnum(character);
    });
    
    if(found != name.end()){
        throw std::runtime_error("Invalid command name.");
    }

    std::transform(name.begin(), name.end(), std::back_insert_iterator(this->_name), [](char c) {
        return std::tolower(c);
    });
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

const std::string& CommandName::operator*() {
    return this->_name;
}

CommandName::operator std::string() const {
    return this->_name;
}

bool CommandName::operator==(const CommandName& other) const {
    return this->_name == other._name;
}

bool CommandName::operator==(const std::string& other) const {
    return this->_name == other;
}

bool CommandName::operator!=(const CommandName& other) const {
    return !(*this == other);
}

bool CommandName::operator!=(const std::string& other) const {
    return !(*this == other);
}
