#pragma once

#include "GameManager.h"
#include <arepa/server/Server.h>

#include <string>
#include <vector>
#include <string>
#include <unordered_map>
#include <iterator>

class GameManager;

namespace commandSpace {


//user input date type for abstraction 
using input = std::string;



enum commandType {
    listMember,     //lists all members that are in lobby/room
    listRoom,       //lists all rooms created in lobby
    createRoom,     //creates a room in lobby
    joinRoom,       //joins a room
    kickUser,       //kicks a user from room/lobby (maintener/room creater)
    quitFromServer, //user quits from server
    initGame,       //start a game
    shutdownServer, //shutdown server
    clear,          //command issued to client to clear view
    message,        //regular message 
    nullCommand     //undefined command. return error
};

class Command{
public:
    Command(GameManager &gm);
    Command(const input &message, GameManager &gm);

    void evaluateMessage(const std::string& message);
    commandType& getCommandType();
    std::vector<input> getTokens() const;

private:
        commandType commandRecieved;
        //tokenized user input 
        std::vector<input> userInput;
        //map to connect user input to command

        GameManager& gameManager;

        const std::unordered_map <std::string,commandType> commandMap ={
            {"/member", commandType::listMember},
            {"/room",   commandType::listRoom},
            {"/create", commandType::createRoom},
            {"/join",   commandType::joinRoom},
            {"/kick",   commandType::kickUser},
            {"/quit",   commandType::quitFromServer},
            {"/initgame", commandType::initGame},
            {"/clear", commandType::clear},
            {"/shutdown", commandType::shutdownServer},
        };

};

}