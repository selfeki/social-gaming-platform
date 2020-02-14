#pragma once

#include "GameInstance.h"
#include "command.h"
#include "jsonconfig.h"

#include <algorithm>
#include <exception>
#include <time.h>
#include <unordered_map>
#include <optional>

using g_config = game_config::configuration;
using s_config = server_config::configuration;
/*
*Game_manager class
*/

//uniquely identifying room id
typedef std::string RoomID;

//message return struct, game_manager returns a vector of these to the daemon,
//on for each client that is to be sent a message
template <typename IDType>
struct messageReturn {
    IDType sendTo;
    std::string message;
    bool shouldShutdown;
};


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


template <typename IDType>
class Room {
public:
    Room(IDType _owner, RoomID room_code);
    void playerJoin(IDType player);
    void exitPlayer(IDType player);
    void gameUpdate();
    std::vector<IDType> returnPlayers();
    void configRoomAndGame(const g_config& game_config);
    IDType getOwner();

private:
    std::vector<IDType> players;
    IDType owner;
    int max_players;
    int player_count;
    time_t born;
    RoomID room_id;
    //the game object associated with each room will run the game's DSL interpreter
    // GameInstance game;
};


template <typename IDType>
class GameManager {
public:
    using messageReturnList = std::vector<messageReturn<IDType>>;

    GameManager();

    void
    setUp(const s_config& server_config);

    void
    removePlayer(IDType player, RoomID room);

    void
    addPlayer(IDType player, RoomID room);

    std::vector<IDType> 
    getRoomOfPlayer(IDType player);

    std::vector<IDType>
    getPlayersInRoom(RoomID room);

    messageReturnList
    returnRoomMembersCommand(IDType player_id);

    messageReturnList
    returnRoomCommand(IDType player_id);

    messageReturnList
    createRoomCommand(IDType id);

    messageReturnList
    joinRoomCommand(IDType id, std::string room_id);

    messageReturnList
    kickPlayerCommand(IDType id, std::string id_to_kick);

    //refactoring leave room command to quit from server...
    messageReturnList
    leaveRoomCommand(IDType id);

    messageReturnList
    quitFromServerCommand(IDType id);

    messageReturnList
    shutdownServerCommand(IDType id);


    messageReturnList
    initRoomCommand(IDType id);

    messageReturnList
    handleGameMessage(std::string msg, IDType player);

    messageReturnList
    destroyRoom(IDType id);

    messageReturnList
    whisperCommand(IDType id, std::string recipient_id, std::string);

    //takes player id and return Room instance
    std::unique_ptr<Room<IDType>> playerIDtoRoom(IDType& id);

private:
    //commandSpace::Command<IDType> commands;
    std::vector<IDType> all_players;

    //roomID to room object map
    std::unordered_map<RoomID, Room<IDType>> id_room_map;

    //player ID to username map?
    //std::unordered_map<IDType, std::string> id_player_map;

    //username to player ID
    std::unordered_map<std::string, IDType> userName_id_map;

    //player ID to roomID map
    std::unordered_map<IDType, RoomID> player_room_map;

    int max_players;

    int max_rooms;

    std::optional<IDType> admin;
};
