#include "commands.h"

#include <sstream>
#include <unordered_map>
#include <tuple>

using Arguments = arepa::command::CommandArguments;
using Context = CommandContext;
using Executor = CommandExecutor;
using User = CommandUser;
using arepa::command::lambda_executor;

typedef std::pair<std::optional<std::string>, GameManager::ReturnCode> getRoomUsernameReturnType;
typedef std::pair<std::optional<RoomID>, GameManager::ReturnCode> createRoomReturnType;


std::unordered_map<std::string, std::unique_ptr<CommandExecutor>> COMMAND_MAP;

#define COMMAND(name, fn) \
    { name, lambda_executor<User, Context>(fn) }

void init_commands() {
    COMMAND_MAP.insert(COMMAND("ping", [](Context& game_manager, User& user, const Arguments& args) {
        std::cout << "Received /ping" << std::endl;
        user.send("Pong.");
    }));

    //example of the "members" command. TODO: the rest of the commands.
    COMMAND_MAP.insert(COMMAND("members", [](Context& game_manager, User& user, const Arguments& args) {
                
        std::string text;

        std::optional<RoomID> room_id = game_manager.getRoomIDOfPlayer(*user);
        
        if(!room_id) {
            user.outgoing_message_queue().emplace_back(*user, "You are not in a room");
            return;
        }

        const std::vector<PlayerID>* players = game_manager.getPlayersInRoom(*room_id);

        if(!players) {
            //should probably throw an exception here because in this case everything is messed up
            return;
        }

        text += *room_id + " players: \n";
        for(auto player : *players) {
            getRoomUsernameReturnType username_return = game_manager.getRoomUsernameOfPlayer(player);
            text += *(username_return.first) + "\n";
        }
        
        user.outgoing_message_queue().emplace_back(*user, text);

    }));

    COMMAND_MAP.insert(COMMAND("room", [](Context& game_manager, User& user, const Arguments& args){
        //auto responses = game_manager.returnRoomCommand(*user);
        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("create", [](Context& game_manager, User& user, const Arguments& args){
        createRoomReturnType room_ret = game_manager.createRoom(*user);

        if(room_ret.second != GameManager::ReturnCode::SUCCESS) {
            user.outgoing_message_queue().emplace_back(*user, "Failed to create room.");
            return;
        }

        user.outgoing_message_queue().emplace_back(*user, "Created room " + *(room_ret.first));
        return;

    }));

    COMMAND_MAP.insert(COMMAND("room", [](Context& game_manager, User& user, const Arguments& args){
        //auto responses = game_manager.returnRoomCommand(*user);
        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("join", [](Context& game_manager, User& user, const Arguments& args){
        //auto responses = game_manager.joinRoomCommand(*user, args.front());
        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("kick", [](Context& game_manager, User& user, const Arguments& args){
        //auto responses = game_manager.kickPlayerCommand(*user, args.front());
        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("clear", [](Context& game_manager, User& user, const Arguments& args){
        //auto responses = game_manager.clearCommand(*user);
        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("quit", [](Context& game_manager, User& user, const Arguments& args){
        //auto responses = game_manager.leaveRoomCommand(*user);
        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));   

    COMMAND_MAP.insert(COMMAND("shutdown", [](Context& game_manager, User& user, const Arguments& args){
        //auto responses = game_manager.shutdownServerCommand(*user);
        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));


    COMMAND_MAP.insert(COMMAND("help", [](Context& game_manager, User& user, const Arguments& args) {
        std::stringstream message;

        message << "Help Commands:\n"
                << "  /help        -- view the command list\n"
                << "\n"

                << "Room Commands:\n"
                << "  /create      -- create a new room\n"
                << "  /join [id]   -- join an existing room\n"
                << "  /room        -- get the current room ID\n"
                << "  /member      -- get the current room members\n"
                << "  /quit        -- leave the current room\n"
                << "\n"

                << "Room Admin Commands:\n"
                << "  /kick [user] -- kick a user from the room";

        user.send(message.str());
    }));
}

void invalidCommand(User& user){
    user.send("Invalid command. Command must be alphanumeric. \n");
}
void unknownCommand(User& user){
    user.send("Unknown command. '/help' for command list. \n");
}


