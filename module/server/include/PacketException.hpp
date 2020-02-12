#pragma once

#include <exception>

namespace arepa::server {

/**
 * An exception for packet serialization or deserialization issues.
 */
class PacketException : public std::exception {
public:
    enum Kind {

        /**
         * An invalid or malformed packet was sent to the server.
         */
        MALFORMED,

        /**
         * A packet was casted (deserialized) as the wrong type.
         */
        ILLEGAL_CAST

    };

private:
    Kind _kind;

public:
    PacketException() = delete;
    explicit PacketException(Kind kind);

    [[nodiscard]] Kind kind() const noexcept;
    [[nodiscard]] const char* what() const noexcept override;
};

}
