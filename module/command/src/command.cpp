#include "command.h"

#include "GameManager.h"

//#include "chatserver/chatserver.cpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
//#include "game_manager.h"


using networking::ConnectionId;
//using networking::Message;
using networking::Server;

namespace commandSpace {


//template <class T>
template <typename IDType>
Command<IDType>::Command()
    : commandRecieved(commandType::nullCommand)
    , userInput(" ") {}

/*
* Putting MessageResult in command class for better
* information hiding. The command class is now just an api the
* daemon has to deal with, rather than the command class being 
* designed aruond the daemon. 
*/


/*
//template <class T>
void Command::requestInfoToServer(const commandType& command){
    switch (command){
        case commandType::listMember:
            
            break;
        case commandType::listRoom:

            break;
        case commandType::createRoom:

        break;
        case commandType::joinRoom:

            break;
        case commandType::kickUser:

            break;
        case commandType::nullCommand:
            Command::returnCommandNotFoundError();
            break;   
    }
}
*/

//template <class T>
template <typename IDType>
commandType Command<IDType>::evaluateMessage(const std::string& message) {
    commandType _commandRecieved = commandType::nullCommand;
    userInput = message;
    if (message[0] != '/') {
        _commandRecieved = commandType::message;
    } else if (message == "/member") {
        _commandRecieved = commandType::listMember;
    } else if (message == "/room") {
        _commandRecieved = commandType::listRoom;
    } else if (message == "/create") {
        _commandRecieved = commandType::createRoom;
    } else if (message == "/join") {
        _commandRecieved = commandType::joinRoom;
    } else if (message == "/kick") {
        _commandRecieved = commandType::kickUser;
    } else if (message == "/quit") {
        _commandRecieved = commandType::quitFromServer;
    } else if (message == "/initgame") {
        _commandRecieved = commandType::initGame;
    } else if (message == "/shutdown") {
        _commandRecieved = commandType::shutdownServer;
    } else {
        _commandRecieved = commandType::nullCommand;
    }
    return _commandRecieved;
}
template <typename IDType>
commandType Command<IDType>::getCommandType() const {
    return commandRecieved;
}
template <typename IDType>
std::string Command<IDType>::returnCommandNotFoundError() {
    return "Error. Command :" + userInput + " not found\n";
}

/*
std::ostringstream Command::memberCommand(){
  std::ostringstream result;
  result<<"Command: Member List\n";
  for (auto client : clients){
    result<<client.id<<"\n";
  }
  return result;
} 
*/
template <typename IDType>
std::vector<MessageResult<IDType>>
Command<IDType>::handleCommand(std::string msg_text, IDType sent_from) {
    std::vector<MessageResult<IDType>> message_queue;
    std::ostringstream result;
    commandType command_recieved = evaluateMessage(msg_text);
    IDType sendTo = sent_from;
    bool quit = false;

    switch (command_recieved) {
    case commandType::message:
        result << sent_from.name() << "> " << msg_text << "\n";
        break;
    case commandType::listMember:
        result << sent_from.name() << "> " << msg_text << "\n";
        //result << memberCommand().str();
        break;
    case commandType::quitFromServer:
        //server.disconnect(sent_from);
        break;
    case commandType::shutdownServer:
        std::cout << "Shutting down.\n";
        quit = true;
        break;
    default:
        result << sent_from.name() << "> " << msg_text << "\n";
        result << "Command not defined.\n";
        break;
    }

    message_queue.push_back({ result.str(), quit, sent_from, sendTo });


    return message_queue;
}

template class Command<networking::ConnectionId>;
template struct MessageResult<networking::ConnectionId>;

}
