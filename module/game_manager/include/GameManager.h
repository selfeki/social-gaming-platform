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
    Room(PlayerID _owner, RoomID room_id);
    void playerJoin(PlayerID player_id);
    void playerLeave(PlayerID player_id);
    void gameUpdate();
    std::vector<PlayerID> returnPlayers();
    void configRoomAndGame(const g_config& game_config);
    PlayerID getOwner();

private:
    std::vector<PlayerID> players;
    PlayerID owner;
    int max_players;
    time_t created_at;
    RoomID room_id;
    //the game object associated with each room will run the game's DSL interpreter
    // GameInstance game;
};



class GameManager {
public:

    struct MessageReturn {
        PlayerID sendTo;
        std::string message;
        bool shouldShutdown;
    };


    using MessageReturn = GameManager::MessageReturn;
    using MessageReturnList = std::vector<MessageReturn>;

    GameManager();


    void
    setUp(const s_config& server_config);

    void
    removePlayer(PlayerID player_id, RoomID room_id);

    void
    addPlayer(PlayerID player_id, RoomID room_id);

    std::vector<PlayerID> 
    getRoomOfPlayer(PlayerID player_id);

    std::vector<PlayerID>
    getPlayersInRoom(RoomID player_id);

    MessageReturnList
    returnRoomMembersCommand(PlayerID player_id);

    MessageReturnList
    returnRoomCommand(PlayerID player_id);

    MessageReturnList
    createRoomCommand(PlayerID player_id);

    MessageReturnList
    joinRoomCommand(PlayerID player_id, std::string room_id);

    MessageReturnList
    kickPlayerCommand(PlayerID player_id, std::string id_to_kick);

    //refactoring leave room command to quit from server...
    MessageReturnList
    leaveRoomCommand(PlayerID player_id);

    MessageReturnList
    quitFromServerCommand(PlayerID player_id);

    MessageReturnList
    shutdownServerCommand(PlayerID player_id);

    MessageReturnList
    initRoomCommand(PlayerID player_id);

    MessageReturnList
    handleGameMessage(std::string msg, PlayerID player_id);

    MessageReturnList
    destroyRoom(PlayerID player_id);

    MessageReturnList
    whisperCommand(PlayerID player_id, std::string recipient_id, std::string msg);

    //takes player id and return Room instance
    Room playerIDtoRoom(PlayerID& player_id);

    //forms message return to send a message everyone in the room
    MessageReturnList
    formMessageToRoomMembers(std::string& message, PlayerID& sentFrom, bool shouldShutdown);
    
    /* forms message to all_players
    *  note: all_players variable not implemented, so it doesn't send message to anyone
    */
    MessageReturnList
    formMessageToEveryone(std::string& message, bool shouldShutdown);
    
    //forms message to a single recipient
    MessageReturnList
    formMessageTo (std::string& message, PlayerID& recipent);
    
    //forms message to multiple recipients
    MessageReturnList
    formMessageTo (std::string& message, std::vector<PlayerID>& recipent);

    MessageReturnList
    clearCommand(PlayerID player_id);

    void 
    createRoom (PlayerID owner, RoomID room_id);

    void 
    addPlayerToRoom (PlayerID player_id, RoomID room_id);

    void
    removePlayerFromRoom (PlayerID player_id);


private:
    //commandSpace::Command<PlayerID> commands;
    std::vector<PlayerID> all_players;

    //roomID to room object map
    std::unordered_map<RoomID, Room> roomid_to_room_map;

    //player ID to username map?
    //std::unordered_map<PlayerID, std::string> id_player_map;

    //username to player ID
    std::unordered_map<std::string, PlayerID> username_to_playerid_map;

    //player ID to roomID map
    std::unordered_map<PlayerID, RoomID> playerid_to_roomid_map;

    int max_players;

    int max_rooms;

    std::optional<PlayerID> admin;
};
