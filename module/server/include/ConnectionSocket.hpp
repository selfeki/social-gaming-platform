#pragma once

#include "Packet.hpp"

#include <arepa/networking/SocketAdapter.hpp>

namespace arepa::server {

typedef arepa::networking::SocketAdapter<Packet> ConnectionSocket;

}
