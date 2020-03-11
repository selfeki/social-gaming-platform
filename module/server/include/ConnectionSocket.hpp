#pragma once

#include <arepa/networking/SocketAdapter.hpp>
#include <arepa/protocol/Packet.hpp>

namespace arepa::server {

typedef arepa::networking::SocketAdapter<arepa::protocol::Packet> ConnectionSocket;

}
