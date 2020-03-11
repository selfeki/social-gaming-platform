#pragma once

#include "SessionId.hpp"

#include <boost/uuid/uuid.hpp>
#include <iostream>

namespace arepa::networking {

/**
 * A token representing a unique session ID and secret.
 *
 * The session ID is intended to be public, shared knowledge.
 * The session secret is intended to be used for re-establishing the session on dropped connections.
 */
class SessionToken {

#pragma mark - Types -
public:
    /**
     * A type alias for a token's ID typename.
     */
    using Id = SessionId;

    /**
     * A type alias for a token's auth secret typename.
     */
    using Secret = boost::uuids::uuid;


#pragma mark - Fields -
private:
    Id _id;
    Secret _secret;


#pragma mark - Constructors -
public:
    SessionToken(const Id& id, const Secret& secret);


#pragma mark - Methods -
public:
    /**
     * Returns the session ID.
     * @return The session ID.
     */
    [[nodiscard]] const Id& id() const;

    /**
     * Returns the session secret.
     * This is used for session authentication (e.g. when resuming).
     *
     * @return The session secret.
     */
    [[nodiscard]] const Secret& secret() const;

    /**
     * Generates a new session token.
     * @return The session token.
     */
    [[nodiscard]] static SessionToken generate();


#pragma mark - Operators -
public:
    [[nodiscard]] operator std::string() const;    // NOLINT
    [[nodiscard]] bool operator==(const SessionToken& rhs) const;
    [[nodiscard]] bool operator==(const SessionToken::Id& rhs) const;
    [[nodiscard]] bool operator!=(const SessionToken& rhs) const;
    [[nodiscard]] bool operator!=(const SessionToken::Id& rhs) const;
};

std::ostream& operator<<(std::ostream& lhs, const SessionToken& rhs);

}


#pragma mark - std::hash -
template <>
struct std::hash<arepa::networking::SessionToken> {
    std::size_t operator()(arepa::networking::SessionToken const& token) const noexcept {
        std::hash<arepa::networking::SessionId> hash;
        return hash(token.id());
    }
};
