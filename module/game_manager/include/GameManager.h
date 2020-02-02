#pragma once

#include "cereal.h"
#include "Server.h"
#include <exception>
#include "command.h"

#ifndef _TESTTEMP_H_
#define _TESTTEMP_H_



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
class GameManager {

public:
    GameManager();

    void setUp(json server_config);

    //void removePlayer(IDType id, RoomType room);

    //std::vector<RoomType> getPlayers(RoomType);

    //void addPlayer(IDType id, RoomType room);

private:

    commandSpace::Command<IDType, RoomType> commands; //so that commands class can interact with game server state


};

#endif