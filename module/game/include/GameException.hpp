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
         * Some other exception reason.
         * Details should be provided.
         */
        OTHER

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
