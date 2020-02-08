#include "command.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


/* Command Class 
*   Class created for translating user input to enumerator variable 
*   This class is not intended for handling actual implementation of commands
*/

namespace commandSpace{




    Command::Command() : commandRecieved(commandType::message),userInput(" "){}
    
    //Initializes userInput and commandRecieved according to messageRecieved
    Command::Command(const input &message):userInput(message){
        evaluateMessage(message);
    }

    //sets commandRecieved depending on the messeage recived
    void Command::evaluateMessage(const std::string& message){
        
        if(message[0] != '/'){
            commandRecieved = commandType::message;
        }
        else {
            auto search = commandMap.find(message);
            if(search != commandMap.end()){
                commandRecieved = search->second;
            }
            else{
                commandRecieved = commandType::nullCommand; 
            }
        }
    }

    commandType& Command::getCommandType()  {
        return commandRecieved;
    }
}



