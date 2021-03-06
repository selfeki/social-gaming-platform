#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace arepa::chat {

/**
 * A structured representation of a player's nickname name.
 * This is a wrapper around std::string which enforces certain rules.
 *
 * It can be implicitly casted to std::string, or accessed with the dereference operator.
 */
class UserNickname {
#pragma mark - Fields -
private:
    std::string _name;

#pragma mark - Constructors -
public:
    /**
     * Constructs and normalizes a new player nickname.
     * This will throw an error if the nickname string is invalid.
     *
     * Validation:
     * ^[0-9A-Za-z_\-]+$
     *
     * @param name The player name.
     */
    explicit UserNickname(const std::string& name);
    explicit UserNickname(const char* name);
    explicit UserNickname(const std::string_view& name);


#pragma mark - Methods (Static) -
public:
    /**
     * Parses a player nickname.
     * This is the same as the constructor, but uses an optional instead of throwing.
     *
     * @param name The player nickname.
     * @return An optional of the parsed nickname.
     */
    static std::optional<UserNickname> parse(const std::string& name);
    static std::optional<UserNickname> parse(const std::string_view& name);


#pragma mark - Operators -
public:
    /**
     * Returns the player nickname string.
     * @return The player name.
     */
    [[nodiscard]] const std::string& operator*() const;

    operator std::string() const;

    bool operator==(const std::string& other) const;
    bool operator==(const UserNickname& other) const;
    bool operator!=(const std::string& other) const;
    bool operator!=(const UserNickname& other) const;
};

}


#pragma mark - std::hash -
template <>
struct std::hash<arepa::chat::UserNickname> {
    std::size_t operator()(arepa::chat::UserNickname const& name) const noexcept {
        std::hash<std::string> hash;
        return hash(name);
    }
};
