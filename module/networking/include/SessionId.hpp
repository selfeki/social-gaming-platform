#pragma once

#include <boost/uuid/uuid.hpp>
#include <string>

namespace arepa::networking {

/**
 * A wrapper around boost::uuids::uuid that serves as a session ID.
 */
class SessionId {

#pragma mark - Fields -
private:
    boost::uuids::uuid _uuid;


#pragma mark - Constructors -
public:
    /**
     * Creates a new Session ID.
     * @param uuid The underlying UUID.
     */
    SessionId(boost::uuids::uuid uuid);    //NOLINT


#pragma mark - Methods -
public:
    /**
     * Convert the session ID to a string.
     * @return The session ID string.
     */
    [[nodiscard]] std::string to_string() const;


#pragma mark - Operators -
public:
    [[nodiscard]] explicit operator std::string() const;
    [[nodiscard]] operator boost::uuids::uuid() const;    // NOLINT

    [[nodiscard]] bool operator==(const SessionId& id) const;
    [[nodiscard]] bool operator==(const boost::uuids::uuid& id) const;
    [[nodiscard]] bool operator==(const std::string& id) const;

    [[nodiscard]] bool operator!=(const SessionId& id) const;
    [[nodiscard]] bool operator!=(const boost::uuids::uuid& id) const;
    [[nodiscard]] bool operator!=(const std::string& id) const;
};

std::ostream& operator<<(std::ostream& lhs, const SessionId& rhs);

}
