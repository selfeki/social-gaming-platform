#include "commands.hpp"

#include "EXAMPLE_Simon.hpp"
#include "logging.hpp"

#include <arepa/chat/ChatException.hpp>
#include <arepa/chat/Room.hpp>
#include <arepa/game/GameException.hpp>

using Arguments = arepa::command::CommandArguments;
using arepa::chat::ChatException;
using arepa::chat::Room;
using arepa::chat::User;
using arepa::server::Connection;
using arepa::server::Server;
using arepa::server::ServerManager;

GlobalCommandMap GLOBAL_COMMAND_MAP;    // NOLINT

constexpr const char* SEPARATOR = "============================================================";

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Global Commands -
// ---------------------------------------------------------------------------------------------------------------------

void stub_room_only(GlobalCommandMap& map, std::string name) {
    map.insert(arepa::command::CommandName(name), [name](Connection& connection, const Arguments& args) -> void {
        connection.send_system_message(std::string("You need to be in a room to use /") + name);
    });
}

void init_global_commands(GlobalCommandMap& map, ServerManager& manager, Server& server) {
    stub_room_only(map, "nick");
    stub_room_only(map, "kick");
    stub_room_only(map, "members");

    map.insert("ping", [](Connection& connection, const Arguments& args) -> void {
        connection.send_system_message("Pong.");
    });

    map.insert("clear", [](Connection& connection, const Arguments& args) -> void {
        connection.send_message("/clear");
    });

    map.insert("whoami", [&manager](Connection& connection, const Arguments& args) -> void {
        auto user = manager.find_user(connection.id());

        user->send_system_message("You are: " + std::string(user->name()));
        user->send_system_message("Games:   " + std::to_string(user->stats().games()));
        user->send_system_message("Wins:    " + std::to_string(user->stats().wins()));
    });

    map.insert("create", [&manager](Connection& connection, const Arguments& args) -> void {
        if (manager.find_user_room(connection.id())) {
            connection.send_error_message("You need to leave your room before you can create another one.");
            return;
        }

        auto user = manager.find_user(connection.id());
        auto room = manager.create_room();
        init_room_commands(room->commands, manager);
        connection.send_system_message(std::string("Your room is: ") + std::string(room->id()));
        manager.user_join_room(user, room);

        clout << "User created room."
              << data(std::make_pair("User", std::string(connection.id())))
              << data(std::make_pair("Room", std::string(room->id())))
              << endl;
    });

    map.insert("join", [&manager](Connection& connection, const Arguments& args) -> void {
        if (manager.find_user_room(connection.id())) {
            connection.send_error_message("You need to leave your room before you can join another one.");
            return;
        }

        if (args.size() != 1) {
            connection.send_error_message("Invalid command usage. Example: /join ABCDE");
            return;
        }

        auto room_id = Room::Id::parse(args.front());
        if (!room_id) {
            connection.send_error_message("Invalid room code. Example: 'ABCDE'");
            return;
        }

        auto user = manager.find_user(connection.id());
        auto room = manager.find_room(*room_id);

        if (!room) {
            connection.send_error_message("[" + args.front() + "] The room does not exist.");
            return;
        }

        try {
            manager.user_join_room(user, room);
            connection.send_system_message(std::string("You joined '") + std::string(room->id()) + "'.");
        } catch (ChatException& ex) {
            connection.send_error_message("[" + args.front() + "] " + std::string(ex.what()));
        }
    });

    map.insert("leave", [&manager](Connection& connection, const Arguments& args) -> void {
        if (!manager.find_user_room(connection.id())) {
            connection.send_error_message("You are not in any room.");
            return;
        }

        auto user = manager.find_user(connection.id());
        auto room = manager.find_user_room(user);

        manager.user_leave_room(user, room);
        connection.send_system_message(std::string("You left '") + std::string(room->id()) + "'.");
    });

    // TODO(anyone): Re-create /help
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Room Commands -
// ---------------------------------------------------------------------------------------------------------------------

void init_room_commands(RoomCommandMap& map, ServerManager& manager) {

    map.insert("members", [](Room& room, User& user, const Arguments& args) {
        user.send_system_message("Members:");
        auto owner = room.owner();

        // List members.
        for (auto& member : room.users()) {
            if (owner && owner == member) {
                user.send_system_message(" - " + std::string(member->name()) + " [owner]");
            } else if (member.is_disqualified()) {
                user.send_system_message(" - " + std::string(member->name()) + " [spectator|disqualified]");
            } else if (member.is_waitlisted()) {
                user.send_system_message(" - " + std::string(member->name()) + " [spectator|waitlisted]");
            } else if (member.is_spectator()) {
                user.send_system_message(" - " + std::string(member->name()) + " [spectator]");
            } else {
                user.send_system_message(" - " + std::string(member->name()));
            }
        }
    });

    map.insert("room", [](Room& room, User& user, const Arguments& args) {
        user.send_system_message("Room:       " + std::string(room.id()));
        user.send_system_message("Players:    " + std::to_string(room.player_count()) + " / " + std::to_string(room.player_limit()));
        user.send_system_message("Spectators: " + std::to_string(room.spectator_count()) + " / " + std::to_string(room.spectator_limit()));
    });

    map.insert("nick", [&manager](Room& room, User& user, const Arguments& args) {
        if (args.size() != 1) {
            user.send_error_message("Invalid command usage. Example: /nick name_100");
            return;
        }

        auto nickname = User::Name::parse(args.front());
        if (!nickname) {
            user.send_error_message("Invalid nickname.");
            return;
        }

        try {
            manager.user_set_nickname(manager.find_user(user.id()), *nickname);
        } catch (arepa::chat::ChatException& ex) {
            user.send_error_message(std::string(ex.what()));
        }
    });

    map.insert("kick", [&manager](Room& room, User& user, const Arguments& args) {
        if (args.size() != 1) {
            user.send_error_message("Invalid command usage. Example: /kick john");
            return;
        }

        if (user != *room.owner()) {
            user.send_error_message("You must be a room owner to kick out a user.");
            return;
        }

        auto nickname = User::Name::parse(args.front());
        if (!nickname) {
            user.send_error_message("Invalid name '" + args.front() + "'.");
            return;
        }

        auto user_to_kick = room.find_user(*nickname);
        if (!user_to_kick) {
            user.send_error_message("User '" + args.front() + "' is not in the room.");
            return;
        }

        if (user == *user_to_kick) {
            user.send_error_message("You cannot kick yourself. Use can /leave to exit the room.");
            return;
        }

        room.remove_user(user_to_kick);
        user.send_system_message("You kicked out " + (**nickname) + ".");
    });

    map.insert("option", [](Room& room, User& user, const Arguments& args) {
        if (args.size() != 2) {
            user.send_error_message("Invalid command usage. Example: /option leader john_smith");
            return;
        }

        if (user != *room.owner()) {
            user.send_error_message("You must be a room owner to change an option.");
            return;
        }

        if (!room.is_game_loaded()) {
            user.send_error_message("This room isn't playing a game. Use /load to select a game.");
            return;
        }

        std::string key = args[0];
        std::string value = args[1];

        auto result = room.game_option(key, value);
        if (!result) {
            user.send_error_message(result.error());
        }
    });

    map.insert("options", [](Room& room, User& user, const Arguments& args) {
        if (args.size() != 0) {
            user.send_error_message("Invalid command usage. Example: /options");
            return;
        }

        if (user != *room.owner()) {
            user.send_error_message("You must be a room owner to see the option.");
            return;
        }

        if (!room.is_game_loaded()) {
            user.send_error_message("This room isn't playing a game. Use /load to select a game.");
            return;
        }

        auto options = room.game().list_option_descriptions();
        if (options.empty()) {
            user.send_system_message("This game doesn't have any options.");
            return;
        }

        user.send_system_message("Options:");
        for (const auto& option : options) {
            user.send_system_message("    '" + option.first + "' -- " + option.second);
        }
    });

    map.insert("load", [](Room& room, User& user, const Arguments& args) {
        if (args.size() != 1) {
            user.send_error_message("Invalid command usage. Example: /load simon");
            return;
        }

        if (user != *room.owner()) {
            user.send_error_message("You must be a room owner to load a game.");
            return;
        }

        std::string game = args.front();
        // TODO(anyone): Implement your game loading here.

        // FIXME(anyone): The following is a temporary example game.
        if (game != "simon") {
            user.send_error_message("Unknown game.");
            return;
        } else {
            room.load_game(std::make_unique<EXAMPLE_Simon>());
        }

        // Message.
        user.send_system_message("The game was loaded successfully.");
        user.send_system_message("Use /options to see the game options.");
        user.send_system_message("Use /start to start the game.");
    });

    map.insert("end", [](Room& room, User& user, const Arguments& args) {
        if (args.size() != 0) {
            user.send_error_message("Invalid command usage. Example: /end");
            return;
        }

        if (user != *room.owner()) {
            user.send_error_message("You must be a room owner to end a game.");
            return;
        }

        if (!room.is_game_loaded()) {
            user.send_error_message("This room isn't playing a game. Use /load to select a game.");
            return;
        }

        room.end_game();
        room.broadcast_message(std::string(user.name()) + " ended the game.");
    });

    map.insert("start", [](Room& room, User& user, const Arguments& args) {
        if (args.size() != 0) {
            user.send_error_message("Invalid command usage. Example: /start");
            return;
        }

        if (user != *room.owner()) {
            user.send_error_message("You must be a room owner to start a game.");
            return;
        }

        if (!room.is_game_loaded()) {
            user.send_error_message("This room isn't playing a game. Use /load to select a game.");
            return;
        }

        try {
            room.start_game();
            user.send_system_message("Started the game. Good luck, and have fun!");
        } catch (arepa::game::GameException& ex) {
            user.send_error_message("Failed to start the game. " + std::string(ex.what()));
        }
    });
}
