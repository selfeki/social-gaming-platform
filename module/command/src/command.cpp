

#include "command.h"
#include <fstream>
#include <iostream>
#include <sstream>




/* Command Class 
*   Class created for translating user input to enumerator variable 
*   This class is not intended for handling actual implementation of commands
*/

namespace commandSpace{
    Command::Command(GameManager &gm) : commandRecieved(commandType::message),userInput(std::vector<input>{" "}),gameManager(gm){}
    
    //Initializes userInput and commandRecieved according to messageRecieved
    Command::Command(const input &message, GameManager &gm) : gameManager(gm) {
        std::istringstream iss(message);
        std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter(userInput));
        
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

    std::vector<input> Command::getTokens() const{
        return userInput;
    }
}



