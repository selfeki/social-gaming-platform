#include "commands.hpp"
#include "config.hpp"
#include "logging.hpp"

#include <arepa/command/Command.hpp>
#include <arepa/server/Server.hpp>
#include <arepa/server/ServerLoop.hpp>
#include <arepa/server/ServerManager.hpp>

using arepa::command::Command;
using arepa::server::Client;
using arepa::server::Server;
using arepa::server::ServerLoop;
using arepa::server::ServerManager;
using Packet = arepa::server::Client::packet_type;

void greet_user(arepa::server::Connection& user) {
    user.send_system_message("Welcome to the server. /create or /join a room to get started!");
}

/**
 * Process a client's command.
 *
 * This will first attempt to let the room (and as such, game instance) process the command.
 * If that fails, it will fall back to the global command map.
 *
 * @param manager The game manager.
 * @param client The client.
 * @param command The command to execute.
 */
void process_command(ServerManager& manager, Client& client, Command& command) {
    auto user_id = client.connection->session_id();

    // If the user is in a room, let the room process their commands.
    auto room = manager.find_user_room(user_id);
    if (room && room->process_command(user_id, command)) {
        return;
    }

    // Find the command executor.
    auto executor = GLOBAL_COMMAND_MAP.find(command.name());
    if (!executor) {
        client.connection->send_error_message("Unknown command. Use /help to see a list of commands.");
        return;
    }

    // Execute the command.
    (*executor)->execute(*client.connection, command.arguments());
}

/**
 * Process a client's message.
 * This will defer processing to the client's room (or send a message if unavailable).
 *
 * @param manager The game manager.
 * @param client The client.
 * @param packet The message to process.
 */
void process_message(ServerManager& manager, Client& client, std::string message) {
    auto user_id = client.connection->session_id();
    auto room = manager.find_user_room(client.connection->id());

    if (!room) {
        client.connection->send_system_message("You need to be in a room to send a message! /create or /join a room.");
        return;
    }

    room->process_message(user_id, message);
}

/**
 * Process a client's packet.
 *
 * @param manager The game manager.
 * @param client The client.
 * @param packet The packet to process.
 */
void process_packet(ServerManager& manager, Client& client, Packet& packet) {
    if (!Command::is_command(packet.text)) {
        process_message(manager, client, packet.text);
        return;
    }

    // Parse the command.
    auto command = Command::parse(packet.text);
    if (!command) {
        client.connection->send_error_message("Invalid command. Use /help to see a list of commands.");
        return;
    }

    try {
        process_command(manager, client, *command);
    } catch (std::runtime_error& ex) {
        client.connection->send_error_message("An internal error occurred.");
        clout << ERROR << "Failed to process a user command."
              << data(std::make_pair("Command", packet.text))
              << data(ex.what())
              << endl;
    }
}

int main(int argc, char* argv[]) {
    init_logging();

    // Load the server configuration.
    clout << "Loading server configuration." << endl;
    std::string config_file = (argc < 2) ? DEFAULT_CONFIG_FILE : std::string(argv[1]);
    arepa::Result<serverConfig::Configuration, std::string> config = load_config_from_json(config_file);

    if (!config) {
        clout << ERROR << "Failed to load server configuration."
              << data(config.error())
              << endl;
        return 1;
    }

    // Set up the game manager.
    clout << "Creating game manager." << endl;
    ServerManager manager(*config);

    // Create and start the network server.
    unsigned short port = (*config).port;
    arepa::networking::websocket::Options opts;
    opts.bind_port = port;

    clout << "Creating server instance."
          << data(std::make_pair("Port", opts.bind_port))
          << endl;

    Server server(opts);
    server.on_accept([](std::shared_ptr<arepa::server::Connection> connection) {
        greet_user(*connection);
    });

    server.on_accept([&manager](std::shared_ptr<arepa::server::Connection> connection) {
        // Create the connection's user object.
        auto user = ServerManager::make_user(connection);
        manager.add_user(user);
    });

    server.on_accept([&manager](std::shared_ptr<arepa::server::Connection> connection) {
        // Destroy the connection's user object.
        auto user = manager.find_user(connection->id());
        if (user) {
            manager.remove_user(user);
        }
    });

    clout << "Successfully created server instance." << endl;

    // Initialize commands
    init_global_commands(GLOBAL_COMMAND_MAP, manager, server);

    // Main game server loop.
    clout << "Initialization is complete." << endl;
    ServerLoop main([&main, &server, &manager]() {
        for (auto& client : server.clients()) {
            while (auto packet = client.messages->receive()) {
                process_packet(manager, client, *packet);
            }
        }
    });

    main.start();
    return 0;
}
