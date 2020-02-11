#include "SessionToken.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

using namespace arepa::networking;

using std::ostream;
using std::string;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

SessionToken::SessionToken(const SessionToken::Id& id, const SessionToken::Secret& secret)
    : _id(id)
    , _secret(secret) {
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

SessionToken SessionToken::generate() {
    boost::uuids::random_generator generator;
    return SessionToken(generator(), generator());
}

const SessionToken::Id& SessionToken::id() const {
    return this->_id;
}

const SessionToken::Secret& SessionToken::secret() const {
    return this->_secret;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

SessionToken::operator std::string() const {
    return boost::uuids::to_string(this->_id);
}

bool operator==(const SessionToken& lhs, const SessionToken& rhs) {
    return lhs.id() == rhs.id();
}

bool operator==(const SessionToken& lhs, const SessionToken::Id& rhs) {
    return lhs.id() == rhs;
}

std::ostream& arepa::networking::operator<<(std::ostream& lhs, const SessionToken& rhs) {
    lhs << boost::uuids::to_string(rhs.id());
    return lhs;
}
