
#include "GameManager.h"
#include "Server.h"



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
GameManager<IDType, RoomType>::GameManager() {}

template <typename IDType, typename RoomType>
void GameManager<IDType, RoomType>::setUp(json server_config) {
    throw(GameManagerException("oops", 0, "lol"));

}

template class GameManager<networking::Connection, int>;

/*
*Avoids linking error allows us to use a template. 
*/

