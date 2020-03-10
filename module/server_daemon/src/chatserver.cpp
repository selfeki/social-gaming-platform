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

#include <arepa/log/LogStream.hpp>
#include <arepa/log/LogStreamFactory.hpp>
#include <arepa/log/console/ConsoleAdapter.hpp>
#include <arepa/log/console/FormatterWithColor.hpp>
#include <arepa/command/Command.hpp>
#include <arepa/server/Server.h>
#include <arepa/server/ServerLoop.hpp>

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

arepa::log::LogStreamFactory clout;
using data = arepa::log::LogStream::data<>;
constexpr auto endl = arepa::log::LogStream::endl;

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
enum MessageType {
    COMMAND,
    GAME_CONFIG,
    NORMAL
};


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

    clout << "New connection found: " << c->session_token() << "\n";

    //override game manager and send back server welcome message...
    networkMessageQueue.emplace_back(c->session_id(), "Welcome to the server! Enter a command...");
    //put player in a room
    //game_manager.createRoomCommand();
}

//Called whenenver a client disconnects. Should handle disconneting player from game room
void onDisconnect(shared_ptr<Connection> c) {
    clout << "Connection lost: " << c->session_token() << "\n";
}


void processMessages(const std::deque<Message>& incoming) {
    for(auto& message : incoming) {
        ConnectionId sentFrom = message.connection;

        // If it's not a command, handle it as a game message.
        //game manager refactored so it does not handle constructing messages, must do it here or somewhere else
        if (!Command::is_command(message.text)) {


            std::optional<RoomID> room_id = gameManager.getRoomIDOfPlayer(sentFrom);
            CommandUser user(sentFrom);
            if(!room_id) {
                std::cout<<"Player not in a room\n";
                user.formMessageToSender("You are not in a room. Create a room with '/create', join one with '/join', type '/help' for help.\n");
            } else {
                std::pair<std::optional<std::string>, GameManager::ReturnCode> username_result = gameManager.getRoomUsernameOfPlayer(sentFrom);
                const std::vector<PlayerID>* players = gameManager.getPlayersInRoom(*room_id);
                std::string text = (*(username_result.first) + ": " + message.text);
                user.formMessageToRoom(gameManager, text);
            }
            std::copy(user.outgoing_message_queue().begin(), user.outgoing_message_queue().end(), std::back_inserter(networkMessageQueue));
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
            executor->second->execute(gameManager, user, command->arguments());
        }
        //Create outgoing message queue
        std::copy(user.outgoing_message_queue().begin(), user.outgoing_message_queue().end(), std::back_inserter(networkMessageQueue));
  }
}



using json = nlohmann::json;

serverConfig::Configuration
loadJSONConfigFile(const std::string& filepath) {
    serverConfig::Configuration config;
    std::ifstream s(filepath);
    if (s.fail()) {
        std::cerr << "Error: " << strerror(errno) << endl;
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

void init_logging() {
    auto adapter = std::make_shared<arepa::log::console::ConsoleAdapter>();
    adapter->use_formatter(std::make_unique<arepa::log::console::FormatterWithColor>());
    arepa::log::global = adapter;
}

int main(int argc, char* argv[]) {
    init_logging();

    serverConfig::Configuration server_config;

    if (argc < 2) {
        clout << "Loading server configuration from default config file." << endl;
        server_config = loadJSONConfigFile(default_json);
    } else {
        clout << "Loading server configuration from " << argv[1] << "." << endl;
        server_config = loadJSONConfigFile(argv[1]);
    }

    if (server_config.err) {
        clout << "Failed to load server configuration." << endl;
        return -1;
    }

    //example
    //g_config game = game_config::load_file("templates/game/rps.json", true);
    //clout << game.player_count["min"] << endl;

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

    clout << "Creating server instance."
          << data(std::make_pair("Port", opts.bind_port))
          << endl;

    Server server(opts, &onConnect, &onDisconnect);
    clout << "Successfully created server instance." << endl;

    // Initialize commands
    init_commands();

    /*
     * Main Game Server Loop
     */

    clout << "Initialization is complete." << endl;
    ServerLoop main([&main, &server]() {
        //Get all messages.
        auto incoming = server.receive();

        //Process messages and put them in networkMessagesQueue (global queue)
        processMessages(incoming);

        //if an admin runs a comman to shutdown server, shouldQuit will be set to true
        //shouldQuit = gameMessagesToNetworkMessages();
        server.send(networkMessageQueue);
        networkMessageQueue.clear();

        if (shouldQuit) {
            main.stop();
        }
    });

    main.start();
    return 0;
}
