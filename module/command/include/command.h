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
using output = std::string;



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

    output kickPlayer(PlayerID player_id, input username_to_kick);
    output createRoom(PlayerID player_id, input username_to_kick);
    output joinRoom(PlayerID player_id, input username_to_kick);
    output normalMessage(PlayerID player_id, input username_to_kick);
    output destroyRoom(PlayerID player_id, input username_to_kick);
    output listRoomMembers(PlayerID player_id, input username_to_kick);
    output leaveRoom(PlayerID player_id, input username_to_kick);
    output whisperToPlayer(PlayerID player_id, input username_to_kick);


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

        const std::unordered_map <GameManager::ReturnCode,std::string> messageMap = {
            {GameManager::ReturnCode::NO_PERMISSION, "Error: Permission denied"},
            {GameManager::ReturnCode::ROOM_NOT_EXIST, "Error: room does not exist"},
            {GameManager::ReturnCode::PLAYER_NOT_EXIST, "Error: player does not exist"},
            {GameManager::ReturnCode::ROOM_FULL, "Error: room full"},
            {GameManager::ReturnCode::FAILURE, "Error"},
        };

};

}