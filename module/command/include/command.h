#pragma once

#include "GameManager.h"
#include <arepa/server/Server.h>

#include <string>
#include <vector>
#include <string>
#include <unordered_map>
#include <iterator>
#include <string_view>
#include <optional>
#include <algorithm>

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

enum SuccessCode {
    KICK,
    CREATE_ROOM,
    JOIN_ROOM,
    LEAVE_ROOM
};

enum SendTo {
    BROADCAST_ROOM,
    BROADCAST_SERVER,
    PLAYER_LIST
};

/*
struct MessageReturn {
    MessageReturn(PlayerID player_id, std::string _msg) : sendTo(player_id), msg(_msg) {};
    PlayerID sendTo;
    std::string msg;
};
*/


class Command{
public:



    Command(GameManager &gm);
    Command(const input &message, GameManager &gm);

    void evaluateMessage(const std::string& message);
    commandType getCommandTypeAndSetTokens(const input& message, std::vector<input>& tokens );
    std::vector<input> getTokens() const;
    void kickPlayer(PlayerID player_id, input& username_to_kick, std::deque<networking::Message>& messagesToSend);
    void createRoom(PlayerID player_id, std::deque<networking::Message>& messagesToSend);
    void joinRoom(PlayerID player_id, RoomID room_id, std::deque<networking::Message>& messagesToSend);
    void destroyRoom(PlayerID player_id, input username_to_kick);
    void listRoomMembers(PlayerID player_id, input username_to_kick);
    void leaveRoom(PlayerID player_id, input username_to_kick);
    void whisperToPlayer(PlayerID player_id, input username_to_kick);
    void regularMessage(PlayerID player_id, const std::string& msg, std::deque<networking::Message>& messagesToSend);

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

        const std::unordered_map <GameManager::ReturnCode,std::string> errorMessageMap = {
            {GameManager::ReturnCode::NO_PERMISSION, "Error: Permission denied"},
            {GameManager::ReturnCode::ROOM_NOT_EXIST, "Error: room does not exist"},
            {GameManager::ReturnCode::PLAYER_NOT_EXIST, "Error: player does not exist"},
            {GameManager::ReturnCode::ROOM_FULL, "Error: room full"},
            {GameManager::ReturnCode::FAILURE, "Error"},
        };

        const std::unordered_map <SuccessCode,std::string> successMessageMap = {
            {SuccessCode::KICK, " has been kicked"},
            {SuccessCode::CREATE_ROOM, "Room has been created: "},
            {SuccessCode::JOIN_ROOM, "You have joined room "}, 
            {SuccessCode::LEAVE_ROOM, "You have left room "},
        };


};

}