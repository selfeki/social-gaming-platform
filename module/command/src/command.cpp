#include "command.h"
#include "Server.h"
//#include "chatserver/chatserver.cpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
//#include "game_manager.h"


using networking::Server;
using networking::Connection;
using networking::Message;

namespace commandSpace{

//template <class T>
Command::Command():commandRecieved(commandType::nullCommand), userInput(" "){
}

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
commandType Command::evaluateMessage(const std::string& message){
    //commandType commandRecieved = commandType::nullCommand;
    userInput = message;
    if(message[0] != '/'){
        commandRecieved = commandType::message;
    }
    else if(message == "/member"){
        commandRecieved = commandType::listMember;
    }
    else if(message == "/room"){
        commandRecieved = commandType::listRoom;
    }
    else if(message == "/create"){
        commandRecieved = commandType::createRoom;
    }
    else if(message == "/join"){
        commandRecieved = commandType::joinRoom;
    }
    else if(message == "/kick"){
        commandRecieved = commandType::kickUser;
    }
    else if(message == "/quit"){
        commandRecieved = commandType::quitFromServer;
    }
    else if(message == "/shutdown"){
        commandRecieved = commandType::shutdownServer;
    }
    else {
        commandRecieved = commandType::nullCommand;
    }
    return commandRecieved;
}

commandType Command::getCommandType() const {
    return commandRecieved;
}

std::string Command::returnCommandNotFoundError(){
    return "Error. Command :" +userInput+" not found\n";
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

std::vector<MessageResult> 
Command::handleCommand(std::string msg_text, Connection sent_from ) {
    std::vector<MessageResult> message_queue;
    std::ostringstream result;
    commandType command_recieved = evaluateMessage(msg_text);
    Connection sendTo;
    bool quit = false;

    switch(command_recieved){
        case commandType::message:
            result << sent_from.id << "> " << msg_text << "\n";
            break;
        case commandType::listMember:
            result << sent_from.id << "> " << msg_text << "\n";
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
            result << sent_from.id << "> " << msg_text << "\n";
            result<<"Command not defined.\n";
            break;
    }

    message_queue.push_back({result.str(),quit,sent_from,sendTo});
    

    return message_queue;

}


}


