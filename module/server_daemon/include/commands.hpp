#pragma once

#include <arepa/chat/Room.hpp>
#include <arepa/chat/User.hpp>
#include <arepa/command/CommandMap.hpp>
#include <arepa/command/CommandName.hpp>
#include <arepa/command/Executor.hpp>
#include <arepa/server/Connection.hpp>
#include <arepa/server/Server.hpp>
#include <arepa/server/ServerManager.hpp>

using GlobalCommandMap = arepa::command::CommandMap<arepa::server::Connection&>;
using RoomCommandMap = arepa::chat::Room::CommandMap;

extern GlobalCommandMap GLOBAL_COMMAND_MAP;

void init_global_commands(GlobalCommandMap& map, arepa::server::ServerManager& manager, arepa::server::Server&);
void init_room_commands(RoomCommandMap& map, arepa::server::ServerManager& manager);
