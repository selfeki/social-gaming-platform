#pragma once

#include <boost/uuid/uuid.hpp>
#include <iostream>

namespace arepa::server {

/**
 * A token representing a unique channel ID.
 */
class ChannelToken {

#pragma mark - Types -
public:
    /**
     * A type alias for a token's ID typename.
     */
    typedef boost::uuids::uuid Id;

    /**
     * A type alias for a token's code typename.
     */
    typedef std::string Code;


#pragma mark - Fields -
private:
    Id _id;
    Code _code;


#pragma mark - Constructors -
public:
    ChannelToken(const Id& id);


#pragma mark - Methods -
public:
    /**
     * Returns the channel ID.
     * @return A reference to the channel ID.
     */
    [[nodiscard]] const Id& id() const;

    /**
     * Returns the channel code.
     * This is a five-letter user-friendly code derived from the channel ID.
     *
     * @return The channel code.
     */
    [[nodiscard]] const Code& code() const;

    /**
     * Generates a new channel token.
     * @return The channel token.
     */
    [[nodiscard]] static ChannelToken generate();


#pragma mark - Operators -
public:
    operator std::string() const;    // NOLINT
};

bool operator==(const ChannelToken& lhs, const ChannelToken& rhs);
bool operator==(const ChannelToken& lhs, const ChannelToken::Id& rhs);
bool operator==(const ChannelToken& lhs, const std::string& rhs);
std::ostream& operator<<(std::ostream& lhs, const ChannelToken& rhs);

}
