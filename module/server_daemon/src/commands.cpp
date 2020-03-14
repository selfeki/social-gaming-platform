#include "commands.hpp"

#include "logging.hpp"

#include <arepa/game/GameException.hpp>
#include <arepa/game/Room.hpp>

using Arguments = arepa::command::CommandArguments;
using arepa::game::Player;
using arepa::game::Room;
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
        if (manager.find_player_room(connection.id())) {
            connection.send_error_message("You need to leave your room before you can create another one.");
            return;
        }

        auto player = manager.find_player(connection.id());
        auto room = manager.create_room();
        init_room_commands(room->commands, manager);
        connection.send_system_message(std::string("Your room is: ") + std::string(room->id()));
        manager.player_join_room(*player, room);

        clout << "User created room."
              << data(std::make_pair("User", std::string(connection.id())))
              << data(std::make_pair("Room", std::string(room->id())))
              << endl;
    });

    map.insert("join", [&manager](Connection& connection, const Arguments& args) -> void {
        if (manager.find_player_room(connection.id())) {
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

        auto player = manager.find_player(connection.id());
        auto room = manager.find_room(*room_id);

        if (!room) {
            connection.send_error_message("Could not find room '" + args.front() + "'. Are you sure it exists?");
            return;
        }

        if ((*room)->is_full()) {
            connection.send_error_message("Room '" + args.front() + "' is full.");
            return;
        }

        manager.player_join_room(*player, *room);
        connection.send_system_message(std::string("You joined '") + std::string((*room)->id()) + "'.");
    });

    map.insert("leave", [&manager](Connection& connection, const Arguments& args) -> void {
        if (!manager.find_player_room(connection.id())) {
            connection.send_error_message("You are not in any room.");
            return;
        }

        auto player = manager.find_player(connection.id());
        auto room = manager.find_player_room(*player);

        manager.player_leave_room(*player, *room);
        connection.send_system_message(std::string("You left '") + std::string((*room)->id()) + "'.");
    });

    // TODO(anyone): Re-create /help
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Room Commands -
// ---------------------------------------------------------------------------------------------------------------------

void init_room_commands(RoomCommandMap& map, GameManager& manager) {
    map.insert("members", [](Room& room, Player& player, const Arguments& args) {
        player.send_system_message("Members:");
        auto owner = room.owner();

        // List players.
        for (auto& member : room.players()) {
            if (owner && *owner == member) {
                player.send_system_message(" - " + std::string(member->name()) + " [owner]");
            } else {
                player.send_system_message(" - " + std::string(member->name()));
            }
        }

        // List spectators.
        for (auto& member : room.spectators()) {
            if (owner && *owner == member) {
                player.send_system_message(" - " + std::string(member->name()) + " [owner]");
            } else {
                player.send_system_message(" - " + std::string(member->name()) + " [spectator]");
            }
        }
    });

    map.insert("nick", [&manager](Room& room, Player& player, const Arguments& args) {
        if (args.size() != 1) {
            player.send_error_message("Invalid command usage. Example: /nick name_100");
            return;
        }

        auto nickname = Player::Name::parse(args.front());
        if (!nickname) {
            player.send_error_message("Invalid nickname.");
            return;
        }

        try {
            manager.player_set_nickname(*room.find_player(player.id()), *nickname);
        } catch (arepa::game::GameException& ex) {
            player.send_error_message(std::string(ex.what()));
        }
    });

    // TODO(anyone): Re-create /kick
    map.insert("kick", [&manager](Room& room, Player& player, const Arguments& args){

        if(player != **room.owner()){
            player.send_error_message("You must be a room owner to kick out a user");
            return;
        }

        if(args.size() != 1){
            player.send_error_message("Invalid command usage. Example: /join ABCDE");
            return;
        }

        auto nickname = Player::Name::parse(args.front());
        if(!nickname){
            player.send_error_message("Invalid name " + args.front());
            return;
        }

        auto player_to_kick = room._find_player_or_spectator(*nickname);
        if(!player_to_kick){
            player.send_error_message("Player/Spectator " + args.front() + " is not in the room");
            return;
        }
        if((**player_to_kick).is_spectator())
        {
            room.remove_spectator(**player_to_kick);
        } else {
            room.remove_player(**player_to_kick);
        }

        player.send_system_message("You kicked out " + (**nickname));
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
