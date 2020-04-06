#pragma once

#include <exception>
#include <optional>
#include <string>

namespace arepa::chat {

/**
 * An exception for some chat issue.
 */
class ChatException : public std::exception {
public:
    enum Kind {

        /**
         * The user is not in the game room.
         */
        USER_NOT_FOUND,

        /**
         * The user's nickname was taken already.
         */
        USER_NICKNAME_TAKEN,

        /**
         * The game room is not found.
         */
        ROOM_NOT_FOUND,

        /**
         * The game room is full.
         */
        ROOM_FULL,

        /**
         * That can't be done during a game.
         */
        NOT_ALLOWED_DURING_GAME,

        /**
         * That option is invalid.
         */
        NOT_ALLOWED_OPTION,

        /**
         * That requires a game to do.
         */
        REQUIRES_GAME

    };

private:
    Kind _kind;
    std::string _what;
    std::optional<std::string> _details;

public:
    ChatException() = delete;
    explicit ChatException(Kind kind);
    explicit ChatException(Kind kind, std::string details);

    [[nodiscard]] std::optional<std::string> details() const noexcept;
    [[nodiscard]] Kind kind() const noexcept;
    [[nodiscard]] const char* what() const noexcept override;
};

}
