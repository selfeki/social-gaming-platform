#pragma once

#include "Message.hpp"

#include <arepa/networking/SocketAdapter.hpp>


namespace arepa::protocol {

typedef arepa::networking::SocketAdapter<Message> MessageSocket;

}
