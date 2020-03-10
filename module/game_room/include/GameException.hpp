#pragma once

#include <exception>
#include <optional>
#include <string>

namespace arepa::game::room {

/**
 * An exception for some game issue.
 */
class GameException : public std::exception {
public:
    enum Kind {

        /**
         * The user is not in the game room.
         */
        PLAYER_NOT_FOUND,

        /**
         * The user's nickname was taken already.
         */
        PLAYER_NICKNAME_TAKEN,

        /**
         * The game room is not found.
         */
        ROOM_NOT_FOUND,

        /**
         * The game room is full.
         */
        ROOM_FULL

    };

private:
    Kind _kind;
    std::string _what;
    std::optional<std::string> _details;

public:
    GameException() = delete;
    explicit GameException(Kind kind);
    explicit GameException(Kind kind, std::string details);

    [[nodiscard]] std::optional<std::string> details() const noexcept;
    [[nodiscard]] Kind kind() const noexcept;
    [[nodiscard]] const char* what() const noexcept override;
};

}
