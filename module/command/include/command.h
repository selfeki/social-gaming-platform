#pragma once

#include <string>
#include <vector>
#include "Server.h"
#include "GameManager.h"


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

struct MessageResult {
  std::string result;
  bool shouldShutdown;
  networking::Connection sentFrom;
  networking::Connection sendTo;
  //commandType userCommand;
};

//template <class T>



class Command{

public:
    //Command(gameManager);
    Command();
    std::vector<MessageResult> handleCommand(std::string msg_text, networking::Connection sent_from );

private:
    //GameManager* gameManager;
    commandType commandRecieved;
    std::string userInput;  
    commandType evaluateMessage(const std::string& message);
    void requestInfoToServer(const commandType& command);
    std::string returnCommandNotFoundError();
    commandType getCommandType() const;
    std::ostringstream memberCommand();
};

}