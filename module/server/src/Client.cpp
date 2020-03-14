#include "Client.hpp"

using namespace arepa::server;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Client::Client(std::shared_ptr<Connection> connection)
    : connection(connection)
    , messages(connection->create_packet_queue()) {}
