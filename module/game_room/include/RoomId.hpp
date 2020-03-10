#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace arepa::game::room {

/**
 * A structured representation of a room ID.
 * This is a wrapper around std::string which enforces certain rules.
 *
 * It can be implicitly casted to std::string, or accessed with the dereference operator.
 */
class RoomId {
#pragma mark - Fields -
private:
    std::string _id;


#pragma mark - Constructors -
public:
    /**
     * Constructs and normalizes a new room ID.
     * This will throw an error if the room ID string is invalid.
     *
     * Validation:
     * ^[0-9A-Za-z_\-]+$
     *
     * @param name The player name.
     */
    explicit RoomId(const std::string& name);
    explicit RoomId(const std::string_view& name);


#pragma mark - Methods (Static) -
public:
    /**
     * Generates a new random RoomId.
     * @param length The length of the ID code.
     * @return The room ID.
     */
    static RoomId generate(size_t length);

    /**
     * Parses a room Id.
     * This is the same as the constructor, but uses an optional instead of throwing.
     *
     * @param id The room ID string.
     * @return An optional of the room ID.
     */
    static std::optional<RoomId> parse(const std::string& id);
    static std::optional<RoomId> parse(const std::string_view& id);

#pragma mark - Operators -
public:
    /**
     * Returns the player nickname string.
     * @return The player name.
     */
    [[nodiscard]] const std::string& operator*();

    operator std::string() const;

    bool operator==(const std::string& other) const;
    bool operator==(const RoomId& other) const;
    bool operator!=(const std::string& other) const;
    bool operator!=(const RoomId& other) const;
};

}
