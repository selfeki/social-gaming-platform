
#include "GameManager.h"
#include <exception>


//GameManagerException::GameManagerException(std::string _info, int _err_id, std::string _json_field) 

std::string GameManagerException::getInfo() const {
    return info;    
}
int GameManagerException::getCode() const {
    return error_id;    
}
std::string GameManagerException::getJsonField() const {
    return json_field;    
}

void GameManager::setUp(json server_config) {
    throw(GameManagerException("oops", 0, "lol"));
}

