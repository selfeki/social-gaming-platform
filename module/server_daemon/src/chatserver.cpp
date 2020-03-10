/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////

#include "GameManager.h"
#include "commands.h"
#include "arepa/game_spec/GameSpecification.h"
#include "arepa/serializer/jsonSerializer.h"
#include "arepa/server_config/Config.h"
#include "arepa/game_spec/Rule.h"
#include "commands.h"

#include <arepa/command/Command.hpp>
#include <arepa/server/Server.h>

#include <boost/uuid/uuid_io.hpp>

//#include "dsl_interpreter.h"
//#include "json_parser.h"
#include <atomic>
#include <cstdint>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using arepa::command::Command;
using networking::ConnectionId;
using networking::Message;
using networking::Server;

typedef networking::ConnectionId UniqueConnectionID;
//typedef MessageReturn<UniqueConnectionID> GameManager::MessageReturn;
//typedef GameManager<UniqueConnectionID> GameManagerAlias;


std::atomic<std::uint64_t> unique_connection_id_counter = 0;

//std::vector<Connection> clients;

std::string default_json = "templates/server/default.json";

GameManager gameManager;


//messages ready to be sent to networking
std::deque<Message> networkMessageQueue;


//message types possible (tentative)
enum MessageType { COMMAND,
    GAME_CONFIG,
    NORMAL };


MessageType getMessageType(const std::string& _message) {
    if (_message[0] == '/') {    //it is a command
        return MessageType::COMMAND;
    } else {    //it is a regular message
        return MessageType::NORMAL;
    }
}

//function to format a message object to a particular client
std::deque<Message> sendToClient(const ConnectionId& client, const std::string& log) {
    std::deque<Message> outgoing;
    outgoing.push_back({ client, log });
    return outgoing;
}

//Whenever a client connections, this function is called it inserts connection and connection id into map

void onConnect(shared_ptr<Connection> c) {
    auto id = ++unique_connection_id_counter;

    std::cout << "New connection found: " << c->session_token() << "\n";

    //override game manager and send back server welcome message...
    networkMessageQueue.emplace_back(c->session_id(), "Welcome to the server! Enter a command...");
    //put player in a room
    //game_manager.createRoomCommand();
}

//Called whenenver a client disconnects. Should handle disconneting player from game room
void onDisconnect(shared_ptr<Connection> c) {
    std::cout << "Connection lost: " << c->session_token() << "\n";

    //command.leaveRoom(c->session_id(), networkMessageQueue);
}


void processMessages(const std::deque<Message>& incoming) {

<<<<<<< HEAD
  for(auto message : incoming) {

    ConnectionId sentFrom = message.connection;

    // If it's not a command, handle it as a game message.

    //game manager refactored so it does not handle constructing messages, must do it here or somewhere else
    if (!Command::is_command(message.text)) {

        std::string text;

        std::optional<RoomID> room_id = gameManager.getRoomIDOfPlayer(sentFrom);

        if(!room_id) {
          text += "You are not in a room. Join one with /join, type /help for help.";
          networkMessageQueue.emplace_back(sentFrom, text);
        } else {
          std::pair<std::optional<std::string>, GameManager::ReturnCode> username_result = gameManager.getRoomUsernameOfPlayer(sentFrom);
          const std::vector<PlayerID>* players = gameManager.getPlayersInRoom(*room_id);
          text += (*(username_result.first) + ": " + message.text);

          for(auto player : *players) {
            networkMessageQueue.emplace_back(player, text);
          }

        }
        continue;
    }

    // If it is a command, parse it and execute it.
    auto command = Command::parse(message.text);
    if (!command) {
        // This means the command string was invalid (not alphanumeric command name).
        // TODO(nikolkam): Send the user an error message.
        std::cout << "Invalid command: " << message.text << std::endl;
        continue;
    }

    // Get the command executor.
    auto executor = COMMAND_MAP.find(command->name());
    if (executor == COMMAND_MAP.end()) {
        // This means the command couldn't be found.
        // TODO(nikolkam): Send the user an error message.
        std::cout << "Unknown command: " << message.text << std::endl;
        continue;
    }

    // Execute the command.
    CommandUser user(sentFrom);
    executor->second->execute(gameManager, user, command->arguments());
    std::copy(user.outgoing_message_queue().begin(), user.outgoing_message_queue().end(), std::back_inserter(networkMessageQueue));
  }
}


=======
    for (auto message : incoming) {

        ConnectionId sentFrom = message.connection;

        // If it's not a command, handle it as a game message.

        //game manager refactored so it does not handle constructing messages, must do it here or somewhere else
        if (!Command::is_command(message.text)) {

            std::string text;

            std::optional<RoomID> room_id = gameManager.getRoomIDOfPlayer(sentFrom);

            if (!room_id) {
                text += "You are not in a room. Join one with /join, type /help for help.";
                networkMessageQueue.emplace_back(sentFrom, text);
            } else {
                std::pair<std::optional<std::string>, GameManager::ReturnCode> username_result = gameManager.getRoomUsernameOfPlayer(sentFrom);
                const std::vector<PlayerID>* players = gameManager.getPlayersInRoom(*room_id);
                text += (*(username_result.first) + ": " + message.text);

                for (auto player : *players) {
                    networkMessageQueue.emplace_back(player, text);
                }
            }
            continue;
        }

        // If it is a command, parse it and execute it.
        auto command = Command::parse(message.text);
        if (!command) {
            // This means the command string was invalid (not alphanumeric command name).
            // TODO(nikolkam): Send the user an error message.
            std::cout << "Invalid command: " << message.text << std::endl;
            continue;
        }

        // Get the command executor.
        auto executor = COMMAND_MAP.find(command->name());
        if (executor == COMMAND_MAP.end()) {
            // This means the command couldn't be found.
            // TODO(nikolkam): Send the user an error message.
            std::cout << "Unknown command: " << message.text << std::endl;
            continue;
        }

        // Execute the command.
        CommandUser user(sentFrom);
        executor->second->execute(gameManager, user, command->arguments());
        std::copy(user.outgoing_message_queue().begin(), user.outgoing_message_queue().end(), std::back_inserter(networkMessageQueue));
    }
}

>>>>>>> origin/selfeki/interpreter

using json = nlohmann::json;

serverConfig::Configuration
loadJSONConfigFile(const std::string& filepath) {
    serverConfig::Configuration config;
    std::ifstream s(filepath);
    if (s.fail()) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return config;
    }

    nlohmann::json jsonServerConfig;
    try {
        jsonServerConfig = json::parse(s);
    } catch (json::parse_error& e) {
        std::cerr << "Invalid JSON server config\n in jsonspec.cpp\n"
                  << "message: " << e.what();
        return config;
    }

    config = jsonSerializer::parseServerConfig(jsonServerConfig);
    config.err = false;



    return config;
}


int main(int argc, char* argv[]) {
    nlohmann::json jsonFile;
    std::ifstream s("/home/at/Downloads/social-gaming/templates/game/simplified.json");
    jsonFile = json::parse(s);
    jsonSerializer::parseRule(jsonFile);
    /* at place this here
    serverConfig::Configuration server_config;

    if (argc < 2) {
        server_config = loadJSONConfigFile(default_json);
    } else {
        server_config = loadJSONConfigFile(argv[1]);
    }

    if (server_config.err) {
        return -1;
    }
 at*/
    //example
    //g_config game = game_config::load_file("templates/game/rps.json", true);
    //std::cout << game.player_count["min"] << std::endl;

    /*
  Try to configurate game_manager... with custom error handling to give useful
  error messages?
  */
    //at try {
        //game_manager.setUp(server_config);
    //at } catch (... /*const GameManagerException& e*/) {
    //at    std::cerr << "Server configuration failed";
        //<< e.what() << '\n'
        //<< e.where() << '\n,
   //at }
   /*at
    unsigned short port = server_config.port;
    arepa::networking::websocket::Options opts;
    opts.bind_port = port;

    Server server(opts, &onConnect, &onDisconnect);
<<<<<<< HEAD
 at*/
=======

    // Initialize commands
    init_commands();

>>>>>>> origin/selfeki/interpreter
    /*
  * Main Game Server Loop
  */
/*at
    while (true) {
        bool shouldQuit = false;


        //Get all messages.
        auto incoming = server.receive();

        //Process messages and put them in networkMessagesQueue (global queue)
        processMessages(incoming);

        //if an admin runs a comman to shutdown server, shouldQuit will be set to true
        //shouldQuit = gameMessagesToNetworkMessages();
        server.send(networkMessageQueue);
        networkMessageQueue.clear();

        if (shouldQuit) {
            break;
        }

        // Sleep to not burn out the CPU.
        constexpr int ONE_HUNDRED_MILLISECONDS = 1000 * 100;
        usleep(ONE_HUNDRED_MILLISECONDS);
    }at*/
    return 0;
}
