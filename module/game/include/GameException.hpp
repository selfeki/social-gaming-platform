#pragma once

#include <exception>
#include <optional>
#include <string>

namespace arepa::game {

/**
 * An exception for some game issue.
 */
class GameException : public std::exception {
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
         * The player is already a spectator in the room.
         * Remove them as a spectator before trying to add them as a player.
         */
        PLAYER_IS_SPECTATOR,

        /**
         * The spectator is already a player in the room.
         * Remove them as a player before trying to add them as a spectator.
         */
        SPECTATOR_IS_PLAYER,

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
