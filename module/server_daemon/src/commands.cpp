#include "commands.hpp"

using Arguments = arepa::command::CommandArguments;
using arepa::game::Player;
using arepa::server::Connection;
using arepa::server::Server;

GlobalCommandMap GLOBAL_COMMAND_MAP;    // NOLINT

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Global Commands -
// ---------------------------------------------------------------------------------------------------------------------

void init_global_commands(GlobalCommandMap& map, GameManager& manager, Server& server) {
    map.insert("ping", [](Connection& connection, const Arguments& args) -> void {
        connection.send_message("Pong.");
    });
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Room Commands -
// ---------------------------------------------------------------------------------------------------------------------

void init_room_commands(RoomCommandMap& map, GameManager& manager) {
}


//void init_commands() {
//    COMMAND_MAP.insert(COMMAND("ping", [](Context& game_manager, User& user, const Arguments& args) {
//        std::cout << "Received /ping" << std::endl;
//        user.formMessageToSender("Pong.");
//    }));
//
//    //example of the "members" command. TODO: the rest of the commands.
//    COMMAND_MAP.insert(COMMAND("members", [](Context& game_manager, User& user, const Arguments& args) {
//        std::stringstream result;
//
//        std::optional<RoomID> room_id = game_manager.getRoomIDOfPlayer(*user);
//
//        if (!room_id) {
//            result << "You are not in a room" << std::endl;
//            user.formMessageToSender(result.str());
//            return;
//        }
//
//        const std::vector<PlayerID>* players = game_manager.getPlayersInRoom(*room_id);
//
//        if (!players) {
//            result << "No one in the room" << std::endl;
//
//        } else {
//            result << " Players in room " << (*room_id) << std::endl;
//            for (auto player : *players) {
//                getRoomUsernameReturnType username_return = game_manager.getRoomUsernameOfPlayer(player);
//                result << *(username_return.first) << std::endl;
//            }
//        }
//        user.formMessageToSender(result.str());
//    }));
//
//
//    COMMAND_MAP.insert(COMMAND("create", [](Context& game_manager, User& user, const Arguments& args) {
//        createRoomReturnType room_ret = game_manager.createRoom(*user);
//
//        if (room_ret.second != GameManager::ReturnCode::SUCCESS) {
//            user.outgoing_message_queue().emplace_back(*user, "Failed to create room.");
//            return;
//        }
//
//        game_manager.addPlayerToRoom(*user, *(room_ret.first));
//        user.formMessageToSender("Created room " + *(room_ret.first));
//
//        return;
//    }));
//
//    COMMAND_MAP.insert(COMMAND("room", [](Context& game_manager, User& user, const Arguments& args) {
//        std::stringstream result;
//        auto roomIDList = game_manager.getRoomIdToRoomMap();
//        if (roomIDList.empty()) {
//            result << "There is no room so far" << std::endl;
//        } else {
//            result << "Room List" << std::endl;
//            for (auto roomID : roomIDList) {
//                result << roomID.first << std::endl;
//            }
//        }
//        user.formMessageToSender(result.str());
//    }));
//
//    COMMAND_MAP.insert(COMMAND("join", [](Context& game_manager, User& user, const Arguments& args) {
//        std::stringstream result;
//        if (args.empty()) {
//            result << "You need to provide room id as an argument eg. /join 'roomid'" << std::endl;
//            user.formMessageToSender(result.str());
//            return;
//        }
//
//        RoomID room = args.front();
//        GameManager::ReturnCode returnCode = game_manager.addPlayerToRoom(*user, room);
//        if (returnCode == GameManager::ReturnCode::FAILURE) {
//            result << "You couldn't enter the room" << std::endl;
//            user.formMessageToSender(result.str());
//        } else {
//            result << (*user).name() << " entered the room" << std::endl;
//            user.formMessageToRoom(game_manager, result.str());
//        }
//    }));
//
//    COMMAND_MAP.insert(COMMAND("kick", [](Context& game_manager, User& user, const Arguments& args) {
//        //auto responses = game_manager.kickPlayerCommand(*user, args.front());
//        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
//        //return responses;
//    }));
//
//    COMMAND_MAP.insert(COMMAND("clear", [](Context& game_manager, User& user, const Arguments& args) {
//        //auto responses = game_manager.clearCommand(*user);
//        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
//        //return responses;
//    }));
//
//    COMMAND_MAP.insert(COMMAND("quit", [](Context& game_manager, User& user, const Arguments& args) {
//        //auto responses = game_manager.leaveRoomCommand(*user);
//        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
//        //return responses;
//    }));
//
//    COMMAND_MAP.insert(COMMAND("shutdown", [](Context& game_manager, User& user, const Arguments& args) {
//        //auto responses = game_manager.shutdownServerCommand(*user);
//        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
//        //return responses;
//    }));
//
//
//    COMMAND_MAP.insert(COMMAND("help", [](Context& game_manager, User& user, const Arguments& args) {
//        std::stringstream message;
//
//        message << "Help Commands:\n"
//                << "  /help        -- view the command list\n"
//                << "\n"
//
//                << "Room Commands:\n"
//                << "  /create      -- create a new room\n"
//                << "  /join [id]   -- join an existing room\n"
//                << "  /room        -- get the current room ID\n"
//                << "  /member      -- get the current room members\n"
//                << "  /quit        -- leave the current room\n"
//                << "\n"
//
//                << "Room Admin Commands:\n"
//                << "  /kick [user] -- kick a user from the room";
//
//        user.formMessageToSender(message.str());
//    }));
//}
//
//void invalidCommand(User& user) {
//}
//
//void unknownCommand(User& user) {
//}
