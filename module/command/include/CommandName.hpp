#pragma once

#include <string>
#include <string_view>

namespace arepa::command {

/**
 * A structured representation of a command name.
 * This is a wrapper around std::string which enforces certain rules.
 *
 * It can be implicitly casted to std::string, or accessed with the dereference operator.
 */
class CommandName {
#pragma mark - Fields -
private:
    std::string _name;

#pragma mark - Constructors -
public:
    /**
     * Constructs and normalizes a new command name.
     * This will throw an error if the command name string is invalid.
     *
     * Validation:
     * ^[0-9a-z]+$
     *
     * Normalization:
     * - Converts to lowercase.
     *
     * @param name The command name.
     */
    explicit CommandName(const std::string& name);
    explicit CommandName(const std::string_view& name);


#pragma mark - Operators -
public:
    /**
     * Returns the command name string.
     * @return The command name string.
     */
    [[nodiscard]] const std::string& operator*();

    operator std::string() const;

    bool operator==(const std::string& other) const;
    bool operator==(const CommandName& other) const;
    bool operator!=(const std::string& other) const;
    bool operator!=(const CommandName& other) const;
};


}
