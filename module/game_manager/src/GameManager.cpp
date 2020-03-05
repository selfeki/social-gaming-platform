
#include "GameManager.h"

#include <arepa/server/Server.h>

typedef std::string RoomID;
typedef networking::ConnectionId PlayerID;



static std::string gen_random(const int len) {
    std::string code = "";

    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        code += (alphanum[rand() % (sizeof(alphanum) - 1)]);
    }
    return code;
}

/*
*GameManagerException class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

GameManagerException::GameManagerException(std::string _info, int _err_id, std::string _json_field)
    : info(_info)
    , error_id(_err_id)
    , json_field(_json_field) {}

std::string GameManagerException::getInfo() const {
    return info;
}
int GameManagerException::getCode() const {
    return error_id;
}
std::string GameManagerException::getJsonField() const {
    return json_field;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*
*Room class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/


Room::Room(PlayerID _owner, RoomID room_code)
    : owner(_owner)
    , room_id(room_code) {
    //configRoomAndGame(game_config);
}


void Room::configRoomAndGame(const g_config& game_config) {
}


const std::vector<PlayerID> &Room::getPlayers() const {
    return players;
}

//if success, return ID of player added, else return nullopt
Room::ReturnCode Room::addPlayer(PlayerID player_id, std::string& username) {   

    auto find = player_id_to_username_map.find(player_id);
    if (find != player_id_to_username_map.end()) return ReturnCode::PLAYER_ALREADY_EXISTS;

    if(username_to_player_id_map.count(username) > 0) return ReturnCode::USERNAME_ALREADY_EXISTS;

    username_to_player_id_map.insert({username, player_id});
    player_id_to_username_map.insert({player_id, username});
    players.push_back(player_id);

    assert(username_to_player_id_map.size() == player_id_to_username_map.size());
    assert(player_id_to_username_map.size() == player.size());
    assert(username_to_player_id_map.at(username) == player_id);
    assert(player_id_to_username_map.at(player_id) == username);

    return ReturnCode::SUCCESS;
}


Room::ReturnCode Room::changePlayerUsername(PlayerID player_id, const std::string& new_username) {
    if(username_to_player_id_map.count(new_username) > 0) {
        return ReturnCode::USERNAME_ALREADY_EXISTS;
    }

    auto find = player_id_to_username_map.find(player_id);
    if (find == player_id_to_username_map.end()) return ReturnCode::PLAYER_NOT_FOUND;

    username_to_player_id_map.erase(find->second);
    username_to_player_id_map.insert({new_username, player_id});
    player_id_to_username_map[player_id] = new_username;

    assert(username_to_player_id_map.size() == player_id_to_username_map.size());
    assert(player_id_to_username_map.size() == player.size());
    assert(username_to_player_id_map.at(username) == player_id);
    assert(player_id_to_username_map.at(player_id) == username);

    return ReturnCode::SUCCESS;
}

//if success, return ID of player removed, else return nullopt
std::optional<PlayerID> Room::removePlayer(PlayerID player_id) {

    auto find = player_id_to_username_map.find(player_id);
    if (find == player_id_to_username_map.end()) return std::nullopt;

    players.erase(std::remove(players.begin(), players.end(), player_id));
    username_to_player_id_map.erase(find->second);
    player_id_to_username_map.erase(player_id);

    assert(username_to_player_id_map.size() == player_id_to_username_map.size());
    assert(player_id_to_username_map.size() == player.size());
    assert(username_to_player_id_map.count(find->second) == 0);
    assert(player_id_to_username_map.count(player_id) == 0);

    return player_id;
}

//update the game's DSL interpreter

void Room::gameUpdate() {
}


const PlayerID Room::getOwner() const{
    return owner;
}

const RoomID& Room::getRoomID() const {
    return room_id;
}

std::optional<std::string> Room::getUsernameFromPlayerID(PlayerID player_id) {
    auto find = player_id_to_username_map.find(player_id);
    if (find == player_id_to_username_map.end()) return std::nullopt;
    return {find->second};
}

std::optional<PlayerID> Room::getPlayerIDFromUsername(const std::string& username) {
    auto find = username_to_player_id_map.find(username);
    if (find == username_to_player_id_map.end()) return std::nullopt;
    return {find->second};
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*
*GameManager class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/


GameManager::GameManager() {
    //commands = commandSpace::Command<PlayerID>();
}


void GameManager::setUp(const s_config& server_config) {
    throw(GameManagerException("oops", 0, "lol"));
}


GameManager::ReturnCode
GameManager::destroyRoom(PlayerID player_id, RoomID room_id) {

    auto find_room = roomid_to_room_map.find(room_id);
    if (find_room == roomid_to_room_map.end()) return GameManager::ReturnCode::ROOM_NOT_EXIST;

    Room& room = find_room->second;

    if(room.getOwner() != player_id) return GameManager::ReturnCode::NO_PERMISSION;

    roomid_to_room_map.erase(room_id);


    for( auto it = playerid_to_roomid_map.begin(); it != playerid_to_roomid_map.end(); ) {
        if (it->second == room_id) {
            it = playerid_to_roomid_map.erase(it);
        } else {
            ++it;
        }
    }

    return GameManager::ReturnCode::SUCCESS;

}


std::optional<RoomID>
GameManager::getRoomIDOfPlayer(PlayerID player_id) {
    auto find_room_id = playerid_to_roomid_map.find(player_id);
    if (find_room_id == playerid_to_roomid_map.end()) return std::nullopt;
 
    return find_room_id->second;
}


Room* GameManager::getRoomFromPlayerID(PlayerID id) {

    auto it_room_id = playerid_to_roomid_map.find(id);
    if(it_room_id == playerid_to_roomid_map.end()) {
        return NULL;
    }
    RoomID wut = (it_room_id->second);
    auto it_room = roomid_to_room_map.find(wut);
    if(it_room == roomid_to_room_map.end()) {
        return NULL;
    }

    return &(it_room->second);
}

Room* GameManager::getRoomFromRoomID(RoomID room_id) {

    auto it_room = roomid_to_room_map.find(room_id);
    if(it_room == roomid_to_room_map.end()) {
        return NULL;
    }

    return &(it_room->second);

}



std::pair<std::optional<RoomID>, GameManager::ReturnCode>
GameManager::createRoom (PlayerID creator){

    RoomID room_code;

    //Generate random key, while that key does not already exist in the map
    do {
        room_code = gen_random(5);
    } while (roomid_to_room_map.count(room_code) > 0);

    roomid_to_room_map.insert({ room_code, Room(creator, room_code) });
    //playerid_to_roomid_map.insert({ creator, room_code });

    if(roomid_to_room_map.count(room_code) == 1) {
        return {room_code, GameManager::ReturnCode::SUCCESS};
    } else {
        return {std::nullopt, GameManager::ReturnCode::FAILURE};
    }

}


GameManager::ReturnCode
GameManager::addPlayerToRoom (PlayerID player_id, RoomID room_id){

    auto find = roomid_to_room_map.find(room_id);
    if (find == roomid_to_room_map.end()) return GameManager::ReturnCode::ROOM_NOT_EXIST;

    Room& room = find->second;
    
    if(true){  //satisfies all conditions eg. room capacity
        Room::ReturnCode result;
        do {
            std::string username_candidate = random_name_generator();
            result = room.addPlayer(player_id, username_candidate);
        } while(result == Room::ReturnCode::USERNAME_ALREADY_EXISTS);

        if(result != Room::ReturnCode::SUCCESS) {
            return GameManager::ReturnCode::FAILURE;
        }

        std::cout << "trace: inside addPlayerToRoom\n";
        playerid_to_roomid_map.insert({ player_id, room_id });
        return GameManager::ReturnCode::SUCCESS;
    }
    
}


GameManager::ReturnCode 
GameManager::removePlayerFromRoom (PlayerID kicking_player_id, PlayerID player_id){

    auto find_room_id = playerid_to_roomid_map.find(player_id);
    if (find_room_id == playerid_to_roomid_map.end()) return GameManager::ReturnCode::PLAYER_NOT_EXIST;

    if(true){ //if action is allowed

        auto find_room = roomid_to_room_map.find(find_room_id->second);
        if (find_room == roomid_to_room_map.end()) return GameManager::ReturnCode::ROOM_NOT_EXIST;

        if((find_room->second).getOwner() != kicking_player_id || kicking_player_id != admin) {
            return GameManager::ReturnCode::NO_PERMISSION;
        }

        if(!(find_room->second).removePlayer(player_id)) return GameManager::ReturnCode::FAILURE;

        playerid_to_roomid_map.erase(player_id);        

        return GameManager::ReturnCode::SUCCESS;
    }
}


std::pair<std::optional<std::string>, GameManager::ReturnCode>
GameManager::getRoomUsernameOfPlayer(PlayerID player_id) {
    
    auto find_room_id = playerid_to_roomid_map.find(player_id);
    if (find_room_id == playerid_to_roomid_map.end()) return {std::nullopt, GameManager::ReturnCode::PLAYER_NOT_EXIST};

    RoomID room_id = (find_room_id->second);

    auto find_room = roomid_to_room_map.find(room_id);
    if (find_room == roomid_to_room_map.end()) return {std::nullopt, GameManager::ReturnCode::ROOM_NOT_EXIST};

    Room& room = find_room->second;

    std::optional<std::string> username = room.getUsernameFromPlayerID(player_id);

    if(!username) return {username, GameManager::ReturnCode::FAILURE};

    return {username, GameManager::ReturnCode::SUCCESS};
}

std::pair<std::optional<PlayerID>, GameManager::ReturnCode> 
GameManager::getPlayerIDFromRoomUsername(const std::string& username, RoomID room_id) {

    auto find_room = roomid_to_room_map.find(room_id);
    if (find_room == roomid_to_room_map.end()) return {std::nullopt, GameManager::ReturnCode::ROOM_NOT_EXIST};

    std::optional<PlayerID> player_id = (find_room->second).getPlayerIDFromUsername(username);

    if(!player_id) return {player_id, GameManager::ReturnCode::FAILURE};

    return {player_id, GameManager::ReturnCode::SUCCESS};
}

const std::vector<PlayerID>* GameManager::getPlayersInRoom(RoomID room_id){
    auto find_room = roomid_to_room_map.find(room_id);
    if (find_room == roomid_to_room_map.end()) return NULL;

    Room& room = (find_room->second);
    const std::vector<PlayerID>& players = room.getPlayers();
    //return &(room.getPlayers());
    return (&players);
}

GameManager::ReturnCode
GameManager::changePlayerUsername(PlayerID player_id, const std::string& new_username) {

    auto find_room_id = playerid_to_roomid_map.find(player_id);
    if (find_room_id == playerid_to_roomid_map.end()) return GameManager::ReturnCode::PLAYER_NOT_EXIST;

    RoomID room_id = (find_room_id->second);

    auto find_room = roomid_to_room_map.find(room_id);
    if (find_room == roomid_to_room_map.end()) return GameManager::ReturnCode::ROOM_NOT_EXIST;

    Room& room = find_room->second;

    Room::ReturnCode ret = room.changePlayerUsername(player_id, new_username);

    if(ret == Room::ReturnCode::SUCCESS){
        return GameManager::ReturnCode::SUCCESS;
    } else if (ret == Room::ReturnCode::USERNAME_ALREADY_EXISTS){
        return GameManager::ReturnCode::USERNAME_ALREADY_EXISTS;
    } else {
        //If this fails, the program is completey effed because things are out of sync...
        return GameManager::ReturnCode::FATAL_ERROR;
    }

}

std::string GameManager::random_name_generator() {
    std::string name;
    static const std::vector<std::string> name_pool = {"aardwolf", "beaver", "lemming", "fox", "baboon", "dragon", "elephant", "sloth"};
    static const std::vector<std::string> adjective_pool = {"super", "iridescent", "bittersweet", "euphoric", "golden", "temporary", "melancholy"};

    name += adjective_pool[rand() % (adjective_pool.size()-1)];
    name += "_";
    name += name_pool[rand() % (name_pool.size()-1)];
    name += std::to_string(rand() % 99);
    
    return name;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`

