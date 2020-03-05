#include "commands.h"
#include <unordered_map>

using Arguments = arepa::command::CommandArguments;
using Context = CommandContext;
using Executor = CommandExecutor;
using User = CommandUser;
using arepa::command::lambda_executor;

std::unordered_map<std::string, std::unique_ptr<CommandExecutor>> COMMAND_MAP;

#define COMMAND(name, fn) \
    { name, lambda_executor<User, Context>(fn) }

void init_commands() {
    COMMAND_MAP.insert(COMMAND("ping", [](Context& game_manager, User& user, const Arguments& args) {
        std::cout << "Received /ping" << std::endl;

    }));

    COMMAND_MAP.insert(COMMAND("member", [](Context& game_manager, User& user, const Arguments& args) {

        
                



        //std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("room", [](Context& game_manager, User& user, const Arguments& args){
        auto responses = game_manager.returnRoomCommand(*user);
        std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("create", [](Context& game_manager, User& user, const Arguments& args){
        auto responses = game_manager.createRoomCommand(*user);
        std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("room", [](Context& game_manager, User& user, const Arguments& args){
        auto responses = game_manager.returnRoomCommand(*user);
        std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("join", [](Context& game_manager, User& user, const Arguments& args){
        auto responses = game_manager.joinRoomCommand(*user, args.front());
        std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("kick", [](Context& game_manager, User& user, const Arguments& args){
        auto responses = game_manager.kickPlayerCommand(*user, args.front());
        std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("clear", [](Context& game_manager, User& user, const Arguments& args){
        auto responses = game_manager.clearCommand(*user);
        std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));

    COMMAND_MAP.insert(COMMAND("quit", [](Context& game_manager, User& user, const Arguments& args){
        auto responses = game_manager.leaveRoomCommand(*user);
        std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));   

    COMMAND_MAP.insert(COMMAND("shutdown", [](Context& game_manager, User& user, const Arguments& args){
        auto responses = game_manager.shutdownServerCommand(*user);
        std::copy(responses.begin(), responses.end(), std::back_inserter(user.outgoing_message_queue()));
        //return responses;
    }));
    
    // TODO(nikolkam): Port the rest of these.
    //                 Also, maybe refactor some of them if you can?

    //        //handle command in game manager
    //        std::vector<messageReturnAlias> cmd_messages;
    //        switch (recieved) {
    //            case commandType::listMember: // `/member`
    //                cmd_messages = game_manager.returnRoomMembersCommand(sentFrom.uuid);
    //                break;
    //            case commandType::listRoom: // `/room`
    //                cmd_messages = game_manager.returnRoomCommand(sentFrom.uuid);
    //                break;
    //            case commandType::createRoom: // `/create`
    //                cmd_messages = game_manager.createRoomCommand(sentFrom.uuid);
    //                break;
    //            case commandType::joinRoom: // `/join`
    //                cmd_messages = game_manager.joinRoomCommand(sentFrom.uuid, tokens[1]);
    //                break;
    //            case commandType::kickUser: // `/kick`
    //                cmd_messages = game_manager.kickPlayerCommand(sentFrom.uuid, tokens[1]);
    //                break;
    //            case commandType::clear: // `/clear`
    //                cmd_messages = game_manager.clearCommand(sentFrom.uuid);
    //                break;
    //            case commandType::quitFromServer: // `/quit`
    //                cmd_messages = game_manager.leaveRoomCommand(sentFrom.uuid);
    //                break;
    //            case commandType::initGame: // `/initgame`
    //                cmd_messages = game_manager.initRoomCommand(sentFrom.uuid);
    //                break;
    //            case commandType::shutdownServer: // `/shutdown`
    //                cmd_messages = game_manager.shutdownServerCommand(sentFrom.uuid);
    //                break;
    //        };
}
