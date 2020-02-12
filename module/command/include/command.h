#pragma once

#include <arepa/server/Server.h>

#include <string>
#include <vector>
#include <string>
#include <unordered_map>
#include <iterator>



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
    message,        //regular message 
    nullCommand     //undefined command. return error
};

class Command{
public:
    Command();
    Command(const input &message);

    void evaluateMessage(const std::string& message);
    commandType& getCommandType();
    std::vector<input> getTokens() const;

private:
        commandType commandRecieved;
        //tokenized user input 
        std::vector<input> userInput;
        //map to connect user input to command
        const std::unordered_map <std::string,commandType> commandMap ={
            {"/member", commandType::listMember},
            {"/room",   commandType::listRoom},
            {"/create", commandType::createRoom},
            {"/join",   commandType::joinRoom},
            {"/kick",   commandType::kickUser},
            {"/quit",   commandType::quitFromServer},
            {"/initgame", commandType::initGame},
            {"/shutdown", commandType::shutdownServer},
        };

};

}