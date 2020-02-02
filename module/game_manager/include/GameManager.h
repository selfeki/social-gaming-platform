#pragma once

#include "cereal.h"
#include "Server.h"
#include <exception>
#include "command.h"
#include <algorithm>
#include <time.h>

#ifndef _TESTTEMP_H_
#define _TESTTEMP_H_



template <typename IDType>
struct messageReturn {
    std::vector<IDType> sendTo;
    std::string message;
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


template <typename IDType, typename RoomType>
class Room {

public:
    void joinPlayer(IDType player);
    void exitPlayer(IDType player);
    void gameUpdate();
private:
    std::vector<IDType> players;
    int max_players;
    int player_count;
    time_t born;


    /*
    * Each room will be associated with a game. The game class is basically the
    * DSL interpreter. 
    */
    //Game game;
};

/*
    


*/



template <typename IDType, typename RoomType>
class GameManager {

public:

    GameManager();

    void setUp(json server_config);

    void removePlayer(IDType player, RoomType room);
    void addPlayer(IDType player, RoomType room);


    std::vector<IDType> getPlayersInRoom(RoomType room);


    //void removePlayer(IDType id, RoomType room);

    //std::vector<RoomType> getPlayers(RoomType);

    //void addPlayer(IDType id, RoomType room);

private:

    commandSpace::Command<IDType, RoomType> commands; //so that commands class can interact with game server state

    std::vector<IDType> all_players;
    std::unordered_map<RoomType, Room<IDType, RoomType>> all_rooms;
    int max_players;
    int max_rooms;
    
};

#endif