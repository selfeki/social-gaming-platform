#include "commands.hpp"

#include "logging.hpp"

#include <arepa/game/GameException.hpp>
#include <arepa/game/Room.hpp>

using Arguments = arepa::command::CommandArguments;
using arepa::game::Room;
using arepa::game::User;
using arepa::server::Connection;
using arepa::server::Server;

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

void init_global_commands(GlobalCommandMap& map, GameManager& manager, Server& server) {
    stub_room_only(map, "nick");
    stub_room_only(map, "kick");
    stub_room_only(map, "members");

    map.insert("ping", [](Connection& connection, const Arguments& args) -> void {
        connection.send_system_message("Pong.");
    });

    map.insert("clear", [](Connection& connection, const Arguments& args) -> void {
        connection.send_message("/clear");
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
        manager.user_join_room(*user, room);

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
            connection.send_error_message("Could not find room '" + args.front() + "'. Are you sure it exists?");
            return;
        }

        if ((*room)->is_full()) {
            connection.send_error_message("Room '" + args.front() + "' is full.");
            return;
        }

        manager.user_join_room(*user, *room);
        connection.send_system_message(std::string("You joined '") + std::string((*room)->id()) + "'.");
    });

    map.insert("leave", [&manager](Connection& connection, const Arguments& args) -> void {
        if (!manager.find_user_room(connection.id())) {
            connection.send_error_message("You are not in any room.");
            return;
        }

        auto user = manager.find_user(connection.id());
        auto room = manager.find_user_room(*user);

        manager.user_leave_room(*user, *room);
        connection.send_system_message(std::string("You left '") + std::string((*room)->id()) + "'.");
    });

    // TODO(anyone): Re-create /help
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Room Commands -
// ---------------------------------------------------------------------------------------------------------------------

void init_room_commands(RoomCommandMap& map, GameManager& manager) {
    map.insert("members", [](Room& room, User& user, const Arguments& args) {
        user.send_system_message("Members:");
        auto owner = room.owner();

        // List users.
        for (auto& member : room.players()) {
            if (owner && *owner == member) {
                user.send_system_message(" - " + std::string(member->name()) + " [owner]");
            } else {
                user.send_system_message(" - " + std::string(member->name()));
            }
        }

        // List spectators.
        for (auto& member : room.spectators()) {
            if (owner && *owner == member) {
                user.send_system_message(" - " + std::string(member->name()) + " [owner]");
            } else {
                user.send_system_message(" - " + std::string(member->name()) + " [spectator]");
            }
        }
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
            manager.user_set_nickname(*room.find_player(user.id()), *nickname);
        } catch (arepa::game::GameException& ex) {
            user.send_error_message(std::string(ex.what()));
        }
    });

    // TODO(anyone): Re-create /kick
    map.insert("kick", [&manager](Room& room, User& user, const Arguments& args) {
        if (user != **room.owner()) {
            user.send_error_message("You must be a room owner to kick out a user.");
            return;
        }

        if (args.size() != 1) {
            user.send_error_message("Invalid command usage. Example: /kick john");
            return;
        }

        auto nickname = User::Name::parse(args.front());
        if (!nickname) {
            user.send_error_message("Invalid name '" + args.front() + "'.");
            return;
        }

        auto user_to_kick = room.find_player_or_spectator(*nickname);
        if (!user_to_kick) {
            user.send_error_message("User '" + args.front() + "' is not in the room.");
            return;
        }

        if (user == **user_to_kick) {
            user.send_error_message("You cannot kick yourself. Use can /leave to exit the room.");
            return;
        }

        room.remove_player_or_spectator(*user_to_kick);
        user.send_system_message("You kicked out " + (**nickname) + ".");
    });
}

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
