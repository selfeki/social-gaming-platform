#include "ChannelToken.hpp"

#include <boost/crc.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

using namespace arepa::server;

using std::ostream;
using std::string;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Static -
// ---------------------------------------------------------------------------------------------------------------------

static char CODE_LUT[] = {
    'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K',
    'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T',
    'V', 'W', 'X', 'Y', 'Z', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '0', 'A'
};


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

ChannelToken::ChannelToken(const ChannelToken::Id& id)
    : _id(id) {

    // Create a CRC32 hash of the Channel ID.
    boost::crc_32_type hash;
    hash.process_bytes(this->_id.data, this->_id.size());
    boost::crc_32_type::value_type hashInt = hash.checksum();

    // Convert the hash to a string.
    // We lose 1 bit of information at the end (for CRC32), but it should be fine.
    char code[sizeof(hashInt) / 6 + 1];
    char* codePtr = static_cast<char*>(static_cast<void*>(&code));
    for (uint8_t i = 0; i < ((sizeof(hashInt) * 8) - 5); i += 5) {
        int lookup = (hashInt >> i) & 0b00011111;
        *(codePtr++) = CODE_LUT[lookup];
    }
    *codePtr = 0;    // Null terminator.

    this->_code = std::string(code);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

ChannelToken ChannelToken::generate() {
    boost::uuids::random_generator generator;
    return ChannelToken(generator());
}

const ChannelToken::Id& ChannelToken::id() const {
    return this->_id;
}

const ChannelToken::Code& ChannelToken::code() const {
    return this->_code;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

ChannelToken::operator std::string() const {
    return boost::uuids::to_string(this->_id);
}

bool operator==(const ChannelToken& lhs, const ChannelToken& rhs) {
    return lhs.id() == rhs.id();
}

bool operator==(const ChannelToken& lhs, const ChannelToken::Id& rhs) {
    return lhs.id() == rhs;
}

bool operator==(const ChannelToken& lhs, const string& rhs) {
    return (boost::uuids::to_string(lhs.id()) == rhs) || (lhs.code() == rhs);
}

std::ostream& arepa::server::operator<<(std::ostream& lhs, const ChannelToken& rhs) {
    lhs << boost::uuids::to_string(rhs.id());
    return lhs;
}
