#pragma once

#include <arepa/networking/SessionId.hpp>

namespace arepa::server {

/**
 * An identifier guaranteed to be unique for every connection.
 */
using ConnectionId = arepa::networking::SessionId;

}
