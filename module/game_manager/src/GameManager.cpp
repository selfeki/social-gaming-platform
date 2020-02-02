
#include "GameManager.h"
#include "Server.h"


/*
*Custom exceptin handling class for the game manager, to give specific error messages about...
*game configuration errors, 
*administration errors
*other...
*/

GameManagerException::GameManagerException(std::string _info, int _err_id, std::string _json_field) : 
    info(_info), error_id(_err_id), json_field(_json_field){}

std::string GameManagerException::getInfo() const {
    return info;    
}
int GameManagerException::getCode() const {
    return error_id;    
}
std::string GameManagerException::getJsonField() const {
    return json_field;    
}

template <typename IDType>
void Room<IDType>::joinPlayer(IDType player) { //todo: should deal with some error handling
    /*
    Add player to the game, if DSL interpeter returns an error, 
    do something (send back error message).
    */

    try {
        //game.addPlayer(player);
    } catch (.../* DSL_interpreter error */) {
    }       
    players.push_back(player);
}

template <typename IDType>
void Room<IDType>::exitPlayer(IDType player) {
    try {
        //game.playerQuit(player);
    } catch (.../* DSL_interpreter error */) {

    }
    std::remove(players.begin(), players.end(), player); //todo: should deal with some error handling
}   

template <typename IDType>
void Room<IDType>::gameUpdate() { //update the game's DSL interpreter
}


template <typename IDType>
GameManager<IDType>::GameManager() {
    commands = commandSpace::Command<IDType>();
}

template <typename IDType>
void GameManager<IDType>::setUp(json server_config) {
    throw(GameManagerException("oops", 0, "lol"));

}

template <typename IDType>
void GameManager<IDType>::removePlayer(IDType player, RoomID room) {

}

template <typename IDType>
void GameManager<IDType>::addPlayer(IDType player, RoomID room) {

}

template <typename IDType>
std::vector<IDType> GameManager<IDType>::getPlayersInRoom(RoomID room) {
    std::vector<IDType> players;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::handleGameMessage(std::string msg, IDType player) {
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list = {player};
    msg_list.push_back(messageReturn<IDType> {player_list, "You are not in a room, please join or create a room.", false});
    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::returnRoomMembersCommand(IDType id){

}
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::returnRoomCommand(IDType id){

}
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::createRoomCommand(IDType id){

}
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::joinRoomCommand(IDType id){

}
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::kickPlayerCommand(IDType id){

}
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::leaveRoomCommand(IDType id){

}
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::initRoomCommand(IDType id){

}





template class GameManager<uintptr_t>;

/*
*Avoids linking error allows us to use a template. 
*/

