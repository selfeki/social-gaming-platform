#pragma once

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
    typedef boost::uuids::uuid Id;

    /**
     * A type alias for a token's auth secret typename.
     */
    typedef boost::uuids::uuid Secret;


#pragma mark - Fields -
private:
    boost::uuids::uuid _id;
    boost::uuids::uuid _secret;


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
    operator std::string() const;    // NOLINT
};

bool operator==(const SessionToken& lhs, const SessionToken& rhs);
bool operator==(const SessionToken& lhs, const SessionToken::Id& rhs);
std::ostream& operator<<(std::ostream& lhs, const SessionToken& rhs);

}
