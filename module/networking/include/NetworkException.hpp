#pragma once

#include <exception>

namespace arepa::networking {

/**
 * An exception for some networking issue.
 */
class NetworkException : public std::exception {
public:
    enum Kind {

        ILLEGAL_BIND_ADDRESS,
        ILLEGAL_BIND_PORT,

        /**
         * Failed to establish a websocket.
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
