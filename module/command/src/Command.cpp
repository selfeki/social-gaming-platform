#include "Command.hpp"

#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace arepa::command;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Command::Command(arepa::command::CommandName name, std::vector<std::string> arguments, std::string raw_arguments)
    : _name(std::move(name))
    , _arguments(std::move(arguments))
    , _raw_arguments(std::move(raw_arguments)) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

const CommandName& Command::name() const {
    return this->_name;
}

const CommandArguments& Command::arguments() const {
    return this->_arguments;
}

const std::string& Command::raw_arguments() const {
    return this->_raw_arguments;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Static) -
// ---------------------------------------------------------------------------------------------------------------------

std::optional<Command> Command::parse(std::string_view input) {
    std::deque<std::string> split;
    boost::split(split, input, boost::is_any_of(" "));

    try {
        // Parse the command name.
        CommandName name = CommandName(split.front().substr(1));
        std::string rawArgs = split.size() == 1 ? "" : std::string(input.substr((split.front()).size() + 1));
        split.pop_front();

        // Parse the command arguments.
        CommandArguments args(split.begin(), split.end());
        return { Command(std::move(name), std::move(args), std::string(rawArgs)) };
    } catch (std::runtime_error& ex) {
        return std::nullopt;
    }
}

bool Command::is_command(const std::string& message) {
    return !message.empty() && message[0] == '/';
}
