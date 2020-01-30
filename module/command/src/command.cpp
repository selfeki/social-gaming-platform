#include "command.h"
#include "Server.h"

#include <string>

namespace commandSpace{
/*
enum commandType {
    listMember,
    listRoom ,
    createRoom ,  
    joinRoom,   
    nullCommand 
};
*/
Command::Command(){
    commandRecieved = commandType::nullCommand;
}

void requestInfoToServer(const commandType& command){
    switch (command){
        case commandType::listMember:
            
            break;
        case commandType::listRoom:

            break;
        case commandType::createRoom:

        break;
        case commandType::joinRoom:

            break;
        case commandType::nullCommand:

            break;
        default:
        
            break;   
    }
}



void evaluateMessage(const std::string& message){
    commandType commandRecieved = commandType::listMember;
    if(message == "/member"){
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
    requestInfoToServer(commandRecieved);
}

}
/*

string message;



void recieveCommand(string& message) {
    if(message.substr(1,6) == "member"){

    }

    
}


void evaluateCommand(auto& message ){
    switch (message){
        case "/member":   ;  
        case "/room":      ;
        case "/join":       ;
    }
}
*/

