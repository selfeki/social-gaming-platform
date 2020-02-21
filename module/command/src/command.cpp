

#include "command.h"
#include <fstream>
#include <iostream>
#include <sstream>




/* Command Class 
*   Class created for translating user input to enumerator variable 
*   This class is not intended for handling actual implementation of commands
*/

namespace commandSpace{
    Command::Command(GameManager &gm) : gameManager(gm){    }
 
    //Initializes userInput and commandRecieved according to messageRecieved
    Command::Command(const input &message, GameManager &gm) : gameManager(gm) {
        
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
    




    commandType Command::getCommandTypeAndSetTokens(const input& message,  std::vector<input>& tokens ) {

        std::istringstream iss(message);
        std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter(tokens));

        if(tokens[0][0] != '/'){
            return commandType::message;
        }
        else {

            auto search = commandMap.find(tokens[0]);
            if(search != commandMap.end()){
                return search->second;
            }
            else{
                return commandType::nullCommand; 
            }
        }
    }




    void
    Command::kickPlayer(PlayerID player_id, input& username_to_kick, std::deque<networking::Message>& messagesToSend){
        
        Room* room = gameManager.getRoomFromPlayerID(player_id);
        if(!room){
            std::string error_msg = errorMessageMap.at(GameManager::ReturnCode::NO_PERMISSION);                  
            messagesToSend.emplace_back(player_id, error_msg);
            return;
        }

        std::optional<PlayerID> id_result = room->getPlayerIDFromUsername(player_id);
        if(!id_result) {
            std::string error_msg = "Player not found";   
            messagesToSend.emplace_back(player_id, error_msg);     
            return;
        }

        GameManager::ReturnCode result = gameManager.removePlayerFromRoom(player_id, *id_result);

        if(result != GameManager::ReturnCode::SUCCESS) {
            std::string error_msg = errorMessageMap.at(result);
            messagesToSend.emplace_back(player_id, error_msg);     
            return;
        }

        const std::vector<PlayerID>& players = room->getPlayers();
        std::string success_msg = username_to_kick + " has been kicked from the room.";
        
        std::for_each(players.begin(), players.end(), [&](auto & p) {
                                                            messagesToSend.emplace_back(p, success_msg);
                                                      });
        
        return;

    }
    void 
    Command::createRoom(PlayerID player_id, std::deque<networking::Message>& messagesToSend){
        
        std::pair<std::optional<RoomID>, GameManager::ReturnCode> create_room_result;
        create_room_result = gameManager.createRoom(player_id);

        if(create_room_result.second != GameManager::ReturnCode::SUCCESS) {
            std::string error_msg = errorMessageMap.at(create_room_result.second);
            messagesToSend.emplace_back(player_id, error_msg); 
            return;    
        }    

        messagesToSend.emplace_back(player_id, "Room " + *(create_room_result.first) + " has been created.");
        return;
    }
    void 
    Command::joinRoom(PlayerID player_id, RoomID room_id, std::deque<networking::Message>& messagesToSend){

        GameManager::ReturnCode result = gameManager.addPlayerToRoom(player_id, room_id);
        std::cout << "trace: addPlayerToRoom called\n";

        if(result != GameManager::ReturnCode::SUCCESS) {
            std::string error_msg = errorMessageMap.at(result);
            messagesToSend.emplace_back(player_id, error_msg); 
            return;    
        }

        Room* room = gameManager.getRoomFromPlayerID(player_id);
        std::optional<std::string> username = room->getUsernameFromPlayerID(player_id);

        if(!username) {
            std::string error_msg = "You do not have a username for some reason.";
            messagesToSend.emplace_back(player_id, error_msg); 
            return;    
        }

        messagesToSend.emplace_back(player_id, "Joined room " + room_id + " as " + *username);
        return;
    }
    void
    Command::destroyRoom(PlayerID player_id, input username_to_kick){

    }
    void
    Command::listRoomMembers(PlayerID player_id, input username_to_kick){

    }
    void
    Command::leaveRoom(PlayerID player_id, input username_to_kick){

    }
    void
    Command::whisperToPlayer(PlayerID player_id, input username_to_kick){

    }

    void
    Command::regularMessage(PlayerID player_id, const std::string& msg, std::deque<networking::Message>& messagesToSend){
        
        Room* room = gameManager.getRoomFromPlayerID(player_id);

        if(!room) {
            std::string error_msg = "You are not in a room. You must be in a room to send normal messages. Try /join <room>.";
            messagesToSend.emplace_back(player_id, error_msg); 
            return;    
        }

        std::optional<std::string> username = room->getUsernameFromPlayerID(player_id);

        if(!username) {
            std::string error_msg = "You do not have a username for some reason. (Try joining the room)";
            messagesToSend.emplace_back(player_id, error_msg); 
            return;               
        }

        const std::vector<PlayerID>& players = room->getPlayers();
        const RoomID& room_id = room->getRoomID();

        std::string success_msg = "{" + room_id + "} " + *username + ": " + msg;
        
        std::for_each(players.begin(), players.end(), [&](auto & p) {
                                                            messagesToSend.emplace_back(p, success_msg);
                                                      });
        return;

    }





}



