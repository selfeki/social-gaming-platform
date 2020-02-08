#pragma once

#include "UnstructuredPacket.hpp"

#include <arepa/networking/SocketAdapter.hpp>

namespace arepa::server {

typedef arepa::networking::SocketAdapter<UnstructuredPacket> ConnectionSocket;

}
