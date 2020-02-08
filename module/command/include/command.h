#pragma once

#include "Server.h"

#include <string>
#include <vector>


namespace commandSpace {

enum commandType {
    listMember,
    listRoom,
    createRoom,
    joinRoom,
    kickUser,
    nullCommand,
    quitFromServer,
    initGame,
    shutdownServer,
    message
};

typedef std::string RoomType;

template <typename IDType>
struct MessageResult {
    std::string result;
    bool shouldShutdown;
    IDType sentFrom;
    IDType sendTo;
    RoomType room;
    //commandType userCommand;
};

//template <class T>

template <typename IDType>
class Command {

public:
    //Command(gameManager);
    Command();
    std::vector<MessageResult<IDType>> handleCommand(std::string msg_text, IDType sent_from);

private:
    /*
    * This commands class will have to have access to
    * the game server state
    */
    commandType commandRecieved;
    std::string userInput;
    commandType evaluateMessage(const std::string& message);
    void requestInfoToServer(const commandType& command);
    std::string returnCommandNotFoundError();
    commandType getCommandType() const;
    std::ostringstream memberCommand();
};

}