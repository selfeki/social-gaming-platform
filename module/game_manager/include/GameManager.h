#pragma once

#include "cereal.h"
#include "Server.h"
#include <exception>
#include "command.h"
#include <algorithm>
#include <time.h>

#ifndef _TESTTEMP_H_
#define _TESTTEMP_H_

typedef uintptr_t RoomID; 

/*
* Struct that the game manager returns to the daemon in response to every parsed message
*/

template <typename IDType>
struct messageReturn {
    std::vector<IDType> sendTo;
    std::string message;
    bool shouldShutdown;
};


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
    void joinPlayer(IDType player);
    void exitPlayer(IDType player);
    void gameUpdate();
private:
    std::vector<IDType> players;
    IDType owner;
    int max_players;
    int player_count;
    time_t born;
    RoomID room_id;
    /*
    * Each room will be associated with a game. The game class is basically the
    * DSL interpreter. 
    */
    //Game game;
};

/*
    


*/



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
    std::vector<messageReturn<IDType>> joinRoomCommand(IDType id);
    std::vector<messageReturn<IDType>> kickPlayerCommand(IDType id);
    std::vector<messageReturn<IDType>> leaveRoomCommand(IDType id);
    std::vector<messageReturn<IDType>> initRoomCommand(IDType id);

    std::vector<messageReturn<IDType>> handleGameMessage(std::string msg, IDType player);


private:

    commandSpace::Command<IDType> commands; //so that commands class can interact with game server state

    std::vector<IDType> all_players;

    std::unordered_map<RoomID, std::unique_ptr<Room<IDType>>> all_rooms;

    int max_players;

    int max_rooms;

    IDType admin;
    
};

#endif