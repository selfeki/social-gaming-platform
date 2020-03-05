#pragma once

#include "GameInstance.h"
#include "arepa/game_spec/GameSpecification.h"
#include "jsonconfig.h"
#include <arepa/server/Server.h>


#include <arepa/command/Executor.hpp>

#include <algorithm>
#include <exception>
#include <optional>
#include <time.h>
#include <unordered_map>

using g_config = game_config::configuration;
using s_config = server_config::configuration;
/*
*Game_manager class
*/

//uniquely identifying room id
typedef std::string RoomID;
typedef networking::ConnectionId PlayerID;

//message return struct, game_manager returns a vector of these to the daemon,
//on for each client that is to be sent a message



//Custom error handling might be good for the future?
class GameManagerException : public std::exception {
public:
    GameManagerException(std::string _info, int _err_id, std::string _json_field);

    std::string
    getInfo() const;

    int
    getCode() const;

    std::string
    getJsonField() const;

private:
    std::string info;
    int error_id;
    std::string json_field;
};



class Room {
public:

    enum ReturnCode {
        SUCCESS = 0,
        ROOM_FULL,
        PLAYER_NOT_FOUND,
        USERNAME_ALREADY_EXISTS,
        PLAYER_ALREADY_EXISTS
    };

    Room(PlayerID _owner, RoomID room_id);
    ReturnCode addPlayer(PlayerID player_id, std::string& username);
    std::optional<PlayerID> removePlayer(PlayerID player_id);
    ReturnCode changePlayerUsername(PlayerID player_id, const std::string& new_username);
    void gameUpdate();
    const std::vector<PlayerID> &getPlayers() const;
    void configRoomAndGame(const g_config& game_config);
    std::optional<std::string> getUsernameFromPlayerID(PlayerID player_id);
    std::optional<PlayerID> getPlayerIDFromUsername(const std::string& username);
    const PlayerID getOwner() const;
    const RoomID& getRoomID() const;

private:
    std::vector<PlayerID> players;
    std::unordered_map<PlayerID, std::string> player_id_to_username_map;
    std::unordered_map<std::string, PlayerID> username_to_player_id_map;
    PlayerID owner;
    int max_players;
    time_t created_at;
    RoomID room_id;
    //the game object associated with each room will run the game's DSL interpreter
    // GameInstance game;
};



class GameManager {
public:

    enum ReturnCode {
        SUCCESS = 0,
        PLAYER_NOT_EXIST,
        ROOM_NOT_EXIST,
        NO_PERMISSION,
        FAILURE, 
        ROOM_FULL, 
        FATAL_ERROR,
        USERNAME_ALREADY_EXISTS
    };

    GameManager();

    void
    setUp(const s_config& server_config);

    //returns room id of player, otherwise returns nullopt there is no such player on the server
    std::optional<RoomID>
    getRoomIDOfPlayer(PlayerID player_id);

    //creates a room. If room creation is successful, returns room_id of room. 
    std::pair<std::optional<RoomID>, ReturnCode> 
    createRoom (PlayerID owner);

    //destroys a room and updates the game manager accordingly
    ReturnCode
    destroyRoom(PlayerID player_id, RoomID room_id);

    //adds a player to a room
    ReturnCode
    addPlayerToRoom (PlayerID player_id, RoomID room_id);

    //removes a player from a room
    ReturnCode
    removePlayerFromRoom (PlayerID kicking_player_id, PlayerID player_id);

    //Gets the username of the plaer in the room. Returns nullopt if player not in room.
    std::pair<std::optional<std::string>, ReturnCode>
    getRoomUsernameOfPlayer(PlayerID player_id);

    //get a player's player_id from their username in a room. 
    std::pair<std::optional<PlayerID>, ReturnCode> 
    getPlayerIDFromRoomUsername(const std::string& username, RoomID room_id);

    //Changes a player's username in a room
    ReturnCode
    changePlayerUsername(PlayerID player_id, const std::string& new_username);

    //returns pointer to vector of players in a room, null pointer if room does not exist.
    const std::vector<PlayerID>* getPlayersInRoom(RoomID room_id);



private:

    static std::string random_name_generator();

    Room* getRoomFromRoomID(RoomID room_id);
    Room* getRoomFromPlayerID(PlayerID id);
    //commandSpace::Command<PlayerID> commands;
    std::vector<PlayerID> all_players;

    //roomID to room object map
    std::unordered_map<RoomID, Room> roomid_to_room_map;

    //player ID to roomID map
    std::unordered_map<PlayerID, RoomID> playerid_to_roomid_map;

    int max_players;

    int max_rooms;

    std::optional<PlayerID> admin;

};
