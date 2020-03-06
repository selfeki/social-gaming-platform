#pragma once

#include <exception>
#include <optional>
#include <string>

namespace arepa::networking {

/**
 * An exception for some networking issue.
 */
class NetworkException : public std::exception {
public:
    enum Kind {

        /**
         * The remote socket closed.
         */
        REMOTE_CONNECTION_CLOSED,

        /**
         * Some remote connection error.
         */
        REMOTE_CONNECTION_ERROR,

        /**
         * Failed to establish a socket.
         */
        ESTABLISH_ERROR,

        /**
         * Failed to bind the server listener.
         */
        BIND_ERROR

    };

private:
    Kind _kind;
    std::string _what;
    std::optional<std::string> _details;

public:
    NetworkException() = delete;
    explicit NetworkException(Kind kind);
    explicit NetworkException(Kind kind, std::string details);

    [[nodiscard]] std::optional<std::string> details() const noexcept;
    [[nodiscard]] Kind kind() const noexcept;
    [[nodiscard]] const char* what() const noexcept override;
};

}
