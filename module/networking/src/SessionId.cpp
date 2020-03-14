#include "SessionId.hpp"

#include <boost/uuid/uuid_io.hpp>

using namespace arepa::networking;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

SessionId::SessionId(boost::uuids::uuid uuid)
    : _uuid(uuid) {}

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

std::string SessionId::to_string() const {
    return boost::uuids::to_string(this->_uuid);
}

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

SessionId::operator boost::uuids::uuid() const {
    return this->_uuid;
}

SessionId::operator std::string() const {
    return this->to_string();
}

bool SessionId::operator==(const SessionId& id) const {
    return this->_uuid == id._uuid;
}

bool SessionId::operator==(const std::string& id) const {
    return this->to_string() == id;
}

bool SessionId::operator==(const boost::uuids::uuid& id) const {
    return this->_uuid == id;
}

bool SessionId::operator!=(const SessionId& id) const {
    return !(*this == id);
}

bool SessionId::operator!=(const std::string& id) const {
    return !(*this == id);
}

bool SessionId::operator!=(const boost::uuids::uuid& id) const {
    return !(*this == id);
}

std::ostream& arepa::networking::operator<<(std::ostream& lhs, const SessionId& rhs) {
    return lhs << rhs.to_string();
}
