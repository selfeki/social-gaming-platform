#pragma once

#include <string>
#include <vector>
#include "Server.h"



namespace commandSpace {

enum commandType {
    listMember,
    listRoom,
    createRoom,
    joinRoom,
    kickUser,   
    nullCommand,
    quitFromServer,
    shutdownServer,
    message
};

template <typename IDType, typename RoomType>
struct MessageResult {
  std::string result;
  bool shouldShutdown;
  IDType sentFrom;
  IDType sendTo;
  RoomType room;
  //commandType userCommand;
};

//template <class T>


template <typename IDType, typename RoomType>
class Command{

public:
    //Command(gameManager);
    Command();
    std::vector<MessageResult<IDType, RoomType>> handleCommand(std::string msg_text, IDType sent_from );

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