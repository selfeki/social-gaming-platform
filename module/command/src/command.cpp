

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

    output Command::kickPlayer(PlayerID player_id, input username_to_kick){
        
        std::optional<RoomID> room_id = gameManager.getRoomIDOfPlayer(player_id);
        if(!room_id) return messageMap.at(GameManager::ReturnCode::PLAYER_NOT_EXIST);

        std::pair<std::optional<PlayerID>, GameManager::ReturnCode> player_id_res = gameManager.getPlayerIDFromRoomUsername(username_to_kick, *room_id);     
        if(player_id_res.second != GameManager::ReturnCode::SUCCESS) return messageMap.at(player_id_res.second);
        
        GameManager::ReturnCode res = gameManager.removePlayerFromRoom(player_id, *(player_id_res.first));
        if(res != GameManager::ReturnCode::SUCCESS) return messageMap.at(res);
        
        return "Kicked player" + username_to_kick;

    }
    output Command::createRoom(PlayerID player_id, input username_to_kick){

    }
    output Command::joinRoom(PlayerID player_id, input username_to_kick){

    }
    output Command::normalMessage(PlayerID player_id, input username_to_kick){

    }
    output Command::destroyRoom(PlayerID player_id, input username_to_kick){

    }
    output Command::listRoomMembers(PlayerID player_id, input username_to_kick){

    }
    output Command::leaveRoom(PlayerID player_id, input username_to_kick){

    }
    output Command::whisperToPlayer(PlayerID player_id, input username_to_kick){

    }





}



