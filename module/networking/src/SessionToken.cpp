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

bool SessionToken::operator==(const SessionToken& rhs) const {
    return this->_id == rhs._id && this->_secret == rhs._secret;
}

bool SessionToken::operator==(const SessionToken::Id& rhs) const {
    return this->_id == rhs;
}

bool SessionToken::operator!=(const SessionToken& rhs) const {
    return !(*this == rhs);
}

bool SessionToken::operator!=(const SessionToken::Id& rhs) const {
    return !(*this == rhs);
}

std::ostream& arepa::networking::operator<<(std::ostream& lhs, const SessionToken& rhs) {
    return lhs << rhs.id();
}
