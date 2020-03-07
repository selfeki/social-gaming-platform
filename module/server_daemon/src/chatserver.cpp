/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////

#include "GameManager.h"
#include "arepa/game_spec/GameSpecification.h"
#include "arepa/serializer/jsonSerializer.h"
#include "arepa/server_config/Config.h"
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
typedef messageReturn<UniqueConnectionID> messageReturnAlias;
typedef GameManager<UniqueConnectionID> GameManagerAlias;


std::atomic<std::uint64_t> unique_connection_id_counter = 0;

//std::vector<Connection> clients;

std::string default_json = "templates/server/default.json";

GameManagerAlias game_manager;


//messages returned from game manager
std::deque<messageReturnAlias> gameMessageQueue;

//messages ready to be sent to networking
std::deque<Message> networkMessageQueue;


/*
transform game messages in game message queue to network messages in
network message queue, then empty the game message queue. These act like queues,
sof first game message processed is first to be put into the network queue.
*/
bool gameMessagesToNetworkMessages() {
    bool quit = false;
    while (!gameMessageQueue.empty()) {

        messageReturnAlias message = gameMessageQueue.front();
        gameMessageQueue.pop_front();
        std::string log = message.message;

        if (message.shouldShutdown) {
            quit = true;
        }

        UniqueConnectionID player = message.sendTo;
        std::cout << boost::uuids::to_string(player.uuid) << "\n";
        std::cout << log << "\n";
        networkMessageQueue.push_back({ player, log });
    }
    return quit;
}


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
}


void processMessages(Server& server, const std::deque<Message>& incoming) {
    for (auto& message : incoming) {
        ConnectionId sentFrom = message.connection;

        // If it's not a command, handle it as a game message.
        if (!Command::is_command(message.text)) {
            std::vector<messageReturnAlias> game_messages = game_manager.handleGameMessage(message.text, sentFrom.uuid);
            for (const auto& game_message : game_messages) {
                gameMessageQueue.push_back(game_message);
            }

            continue;
        }

        CommandUser user(sentFrom);
        auto command = Command::parse(message.text);
        auto executor = COMMAND_MAP.find(command->name());

        // This means the command string was invalid (not alphanumeric command name).
        if (!command) {
            invalidCommand(user);
            std::cout<<"["<<(*user).name()<<"] Invalid command:" << message.text << std::endl;
        }
        // This means the command couldn't be found.
        else if(executor == COMMAND_MAP.end()){
            unknownCommand(user);
            std::cout<<"["<<(*user).name()<<"] Unknown command:" << message.text << std::endl;
        }
        //Execute the command and store the result in user.outgoing_message_queue()
        else
        {
            executor->second->execute(game_manager, user, command->arguments());
        }

        //Create outgoing message queue         
        std::copy(user.outgoing_message_queue().begin(), user.outgoing_message_queue().end(), std::back_inserter(gameMessageQueue));
    }
}



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

    serverConfig::Configuration server_config;

    if (argc < 2) {
        server_config = loadJSONConfigFile(default_json);
    } else {
        server_config = loadJSONConfigFile(argv[1]);
    }

    if (server_config.err) {
        return -1;
    }

    //example
    //g_config game = game_config::load_file("templates/game/rps.json", true);
    //std::cout << game.player_count["min"] << std::endl;

    /*
  Try to configurate game_manager... with custom error handling to give useful
  error messages?
  */
    try {
        //game_manager.setUp(server_config);
    } catch (... /*const GameManagerException& e*/) {
        std::cerr << "Server configuration failed";
        //<< e.what() << '\n'
        //<< e.where() << '\n,
    }

    unsigned short port = server_config.port;
    arepa::networking::websocket::Options opts;
    opts.bind_port = port;

    Server server(opts, &onConnect, &onDisconnect);

    // Initialize commands
    init_commands();

    /*
     * Main Game Server Loop
     */

    while (true) {
        bool shouldQuit = false;


        //Get all messages.
        auto incoming = server.receive();

        //Process messages and put them in gameMessagesQueue (global queue)
        processMessages(server, incoming);

        //if an admin runs a comman to shutdown server, shouldQuit will be set to true
        shouldQuit = gameMessagesToNetworkMessages();
        server.send(networkMessageQueue);
        networkMessageQueue.clear();

        if (shouldQuit) {
            break;
        }

        // Sleep to not burn out the CPU.
        constexpr int ONE_HUNDRED_MILLISECONDS = 1000 * 100;
        usleep(ONE_HUNDRED_MILLISECONDS);
    }
    return 0;
}
