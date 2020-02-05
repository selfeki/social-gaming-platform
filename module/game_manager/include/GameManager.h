#pragma once

#include "cereal.h"
#include "Server.h"
#include <exception>
#include "command.h"
#include <algorithm>
#include <time.h>
#include <unordered_map>

#ifndef _TESTTEMP_H_
#define _TESTTEMP_H_

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
class GameManagerException: public std::exception {
public:
    GameManagerException(std::string _info, int _err_id, std::string _json_field);
    std::string getInfo() const;
    int getCode() const;
    std::string getJsonField() const;
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
    void configRoomAndGame(json game_config);
private:
    std::vector<IDType> players;
    IDType owner;
    int max_players;
    int player_count;
    time_t born;
    RoomID room_id;

    //the game object associated with each room will run the game's DSL interpreter
    //Game game;
};


template <typename IDType>
class GameManager {

public:

    GameManager();
    void setUp(json server_config);
    void removePlayer(IDType player, RoomID room);
    void addPlayer(IDType player, RoomID room);
    std::vector<IDType> getPlayersInRoom(RoomID room);
    std::vector<messageReturn<IDType>> returnRoomMembersCommand(IDType id);
    std::vector<messageReturn<IDType>> returnRoomCommand(IDType id);
    std::vector<messageReturn<IDType>> createRoomCommand(IDType id);
    std::vector<messageReturn<IDType>> joinRoomCommand(IDType id, std::string room_id);
    std::vector<messageReturn<IDType>> kickPlayerCommand(IDType id);
    std::vector<messageReturn<IDType>> leaveRoomCommand(IDType id);
    std::vector<messageReturn<IDType>> initRoomCommand(IDType id);
    std::vector<messageReturn<IDType>> handleGameMessage(std::string msg, IDType player);

private:

    //commandSpace::Command<IDType> commands; 
    std::vector<IDType> all_players;

    //roomID to room object map
    std::unordered_map<RoomID, Room<IDType>> id_room_map;

    //player ID to username map?
    std::unordered_map<IDType, std::string> id_player_map;

    //player ID to roomID map
    std::unordered_map<IDType, RoomID> player_room_map;

    int max_players;
    int max_rooms;
    IDType admin;
};

#endif