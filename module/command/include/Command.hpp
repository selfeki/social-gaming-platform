#pragma once

#include "CommandArguments.hpp"
#include "CommandName.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace arepa::command {

/**
 * A structured representation of a command string.
 * This includes the command name and arguments.
 */
class Command {
#pragma mark - Fields -
private:
    const CommandName _name;
    const std::vector<std::string> _arguments;
    const std::string _raw_arguments;

#pragma mark - Constructors -
public:
    /**
     * Constructs a new command string.
     *
     * @param name The command name.
     * @param arguments The command arguments.
     * @param raw_arguments The raw argument string (raw string without leading command).
     */
    Command(CommandName name, CommandArguments arguments, std::string raw_arguments);


#pragma mark - Methods -
public:
    /**
     * Returns the command name.
     * @return A reference to the command name.
     */
    [[nodiscard]] const CommandName& name() const;

    /**
     * Returns the command arguments.
     * @return A reference to the command arguments.
     */
    [[nodiscard]] const CommandArguments& arguments() const;

#pragma mark - Methods (Static) -
public:
    /**
     * Parses a string into a command string.
     * This assumes the input contains a leading slash.
     *
     * @param input The string.
     * @return The corresponding command string, or optional if the command is malformed (e.g. invalid name)
     */
    [[nodiscard]] static std::optional<Command> parse(std::string_view input);

    /**
     * Checks if a message is a command.
     *
     * @param message The message text.
     * 
     * @return True if the message is a command.
     */
    [[nodiscard]] static bool is_command(const std::string& message);
};


}
