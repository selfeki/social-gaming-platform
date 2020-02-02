
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

template <typename IDType, typename RoomType>
void Room<IDType, RoomType>::joinPlayer(IDType player) { //todo: should deal with some error handling
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

template <typename IDType, typename RoomType>
void Room<IDType, RoomType>::exitPlayer(IDType player) {
    try {
        //game.playerQuit(player);
    } catch (.../* DSL_interpreter error */) {

    }
    std::remove(players.begin(), players.end(), player); //todo: should deal with some error handling
}   

template <typename IDType, typename RoomType>
void Room<IDType, RoomType>::gameUpdate() { //update the game's DSL interpreter
}


template <typename IDType, typename RoomType>
GameManager<IDType, RoomType>::GameManager() {
    commands = commandSpace::Command<IDType, RoomType>();
}

template <typename IDType, typename RoomType>
void GameManager<IDType, RoomType>::setUp(json server_config) {
    throw(GameManagerException("oops", 0, "lol"));

}

template <typename IDType, typename RoomType>
void GameManager<IDType, RoomType>::removePlayer(IDType player, RoomType room) {

}

template <typename IDType, typename RoomType>
void GameManager<IDType, RoomType>::addPlayer(IDType player, RoomType room) {

}

template <typename IDType, typename RoomType>
std::vector<IDType> GameManager<IDType, RoomType>::getPlayersInRoom(RoomType room) {
    std::vector<IDType> players;
}





template class GameManager<networking::Connection, int>;

/*
*Avoids linking error allows us to use a template. 
*/

