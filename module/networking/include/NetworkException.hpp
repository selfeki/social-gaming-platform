#pragma once

#include <exception>

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

public:
    NetworkException() = delete;
    explicit NetworkException(Kind kind);

    [[nodiscard]] Kind kind() const noexcept;
    [[nodiscard]] const char* what() const noexcept override;
};

}
