#include "command.h"
#include "Server.h"
//#include "chatserver/chatserver.cpp"
#include <string>

namespace commandSpace{

//template <class T>
Command::Command():commandRecieved(commandType::nullCommand), userInput(" "){
}

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




}


