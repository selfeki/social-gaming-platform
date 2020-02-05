#include "Message.hpp"

#include <string>
#include <vector>

using namespace arepa::protocol;
using std::string;
using std::uint8_t;
using std::vector;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Message::Message(std::string text)
    : text(text) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

vector<uint8_t> Message::to_bytes(const string& message) {
    return std::vector<uint8_t>(message.begin(), message.end());
}

vector<uint8_t> Message::to_bytes(const Message& message) {
    return Message::to_bytes(message.text);
}

Message Message::from_bytes(const vector<uint8_t>& bytes) {
    if (bytes.empty())
        return Message("");

    // Copy the bytes to the string.
    string text(bytes.begin(), bytes.end());

    // Return the message.
    return Message(std::move(text));
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

Message::operator string() {
    return this->text;
}
