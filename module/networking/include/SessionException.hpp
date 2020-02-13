#pragma once

#include <exception>

namespace arepa::networking {

/**
 * An exception caused by something wrong with the session.
 */
class SessionException : public std::exception {
public:
    enum Kind {

        /**
         * The session is invalid.
         * This can mean:
         *
         * - The session ID wasn't found.
         */
        INVALID_SESSION,

        /**
         * The session secret is invalid.
         * This can mean:
         *
         * - The session secret doesn't match.
         */
        INVALID_SESSION_SECRET

    };

private:
    Kind _kind;

public:
    SessionException() = delete;
    SessionException(Kind kind);

    [[nodiscard]] Kind kind() const noexcept;
    [[nodiscard]] const char* what() const noexcept override;
};

}
