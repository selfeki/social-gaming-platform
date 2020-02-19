
#include "GameManager.h"

#include <arepa/server/Server.h>

typedef std::string RoomID;
typedef networking::ConnectionId PlayerID;

using MessageReturn = GameManager::MessageReturn;

static std::string gen_random_username() {
    std::string name;
    static const std::vector<std::string> name_pool = {"aardwolf", "beaver", "lemming", "fox", "baboon", "dragon", "elephant", "sloth"};
    static const std::vector<std::string> adjective_pool = {"super", "iridescent", "bittersweet", "euphoric", "golden", "temporary", "melancholy"};

    name += name_pool[rand() % (name_pool.size()-1)];
    name += "_";
    name += adjective_pool[rand() % (adjective_pool.size()-1)];
    name += std::to_string(rand());

    return name;
}

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


const std::vector<PlayerID> &Room::returnPlayers() const {
    return players;
}

//if success, return ID of player added, else return nullopt
std::optional<PlayerID> Room::addPlayer(PlayerID player_id, std::string (*random_name_generator)()) {    //todo: should deal with some error handling

    auto find = player_id_to_username_map.find(player_id);
    if (find != player_id_to_username_map.end()) return std::nullopt;

    std::string username;
    do { 
       username = random_name_generator();
    } while(username_to_player_id_map.find(username) != username_to_player_id_map.end());

    username_to_player_id_map.insert({username, player_id});
    player_id_to_username_map.insert({player_id, username});
    players.push_back(player_id);

    assert(username_to_player_id_map.size() == player_id_to_username_map.size());
    assert(player_id_to_username_map.size() == player.size());
    assert(username_to_player_id_map.at(username) == player_id);
    assert(player_id_to_username_map.at(player_id) == username);

    return find->first;
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


PlayerID Room::getOwner() {
    return owner;
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



//std::vector<PlayerID> &GameManager::getPlayersInRoom(RoomID room_id) const {
//    return  
//}

std::vector<MessageReturn> GameManager::handleGameMessage(std::string msg, PlayerID player_id) {


    //Subject to change, we don't have an interpreter working yet.

    std::vector<MessageReturn> msg_list;
    //If player is not in a room, send this message
    if (playerid_to_roomid_map.count(player_id) == 0) {
        std::string text = "You are not in a room, please join or create a room.\n";
        //msg_list = formMessageTo(text,player_id);
    }
    else {
        RoomID room_id = playerid_to_roomid_map.at(player_id);
        std::string text = "Room [" + room_id + "] " + std::string(player_id) + ": " + msg;
        //msg_list = formMessageToRoomMembers(text, player_id, false);
    }
    return msg_list;
}

/*
std::vector<MessageReturn> GameManager::returnRoomMembersCommand(PlayerID player_id){
    std::vector<MessageReturn> msg_list;

    RoomID player_room_id = playerid_to_roomid_map.at(player_id);
    if (roomid_to_room_map.count(player_room_id) == 0) {
        std::string text = "Room " + player_room_id + " does not exist.\n";
        msg_list = formMessageTo(text,player_id);
    } else{
        Room player_room = roomid_to_room_map.at(player_room_id);
        std::vector<PlayerID> players_in_room = player_room.returnPlayers();
        std::string text = "";
        for(auto& player : players_in_room){
            text = text + std::string(player) + "\n";
        }
        msg_list = formMessageTo(text,player_id);
    }
    return msg_list;
}


std::vector<MessageReturn> GameManager::returnRoomCommand(PlayerID player_id) {
    std::vector<MessageReturn> msg_list;

    if (playerid_to_roomid_map.count(player_id) == 0) {
        std::string text = "You are not in a room.\n";
        msg_list = formMessageTo(text,player_id); 
    } else {
        RoomID player_room_id = playerid_to_roomid_map.at(player_id);
        std::string text = "Room ID: " + std::string();
        msg_list = formMessageTo(text,player_id); 
    }
    return msg_list;
}

//Creates a new room.

std::vector<MessageReturn> GameManager::createRoomCommand(PlayerID player_id) {

    std::vector<MessageReturn> msg_list;
    RoomID room_code;

    //Generate random key, while that key does not already exist in the map
    do {
        room_code = gen_random(5);
    } while (roomid_to_room_map.count(room_code) > 0);

    createRoom(player_id, room_code);

    std::string text = "Successfully created room " + room_code + "\n";
    msg_list = formMessageTo(text,player_id);

    return msg_list;
}


std::vector<MessageReturn> GameManager::joinRoomCommand(PlayerID id, std::string room_code) {

    std::vector<MessageReturn> msg_list;
    std::vector<PlayerID> player_list;

    //if room does not exist, send error msg
    if (roomid_to_room_map.count(room_code) == 0) {
        player_list.push_back(id);
        std::string text = "room " + room_code + " does not exist...\n";
        msg_list = formMessageTo(text, id);
    }else if (playerid_to_roomid_map.count(id) > 0) {
        if (playerid_to_roomid_map.at(id) == room_code) {
            player_list.push_back(id);
            std::string text = "You're alread in room " + room_code + "!\n";
            msg_list = formMessageTo(text,id);
            //if player is in a different room, leave that room and join this room
        } else {
            leaveRoomCommand(id);
            //...
        }
        //If player is not in a room, join this room, send message to every player in room that
        //a new player has joined
    }else {
        roomid_to_room_map.at(room_code).playerJoin(id);
        playerid_to_roomid_map.insert({ id, room_code });
        std::string text = string(id) + " has joined room " + room_code + "!\n";
        msg_list = formMessageToRoomMembers(text,id,false);
    }

    return msg_list;
}


std::vector<MessageReturn> GameManager::kickPlayerCommand(PlayerID player_id, std::string id_to_kick) {
    std::vector<MessageReturn> msg_list;
    std::vector<PlayerID> player_list;

    //TODO: find propper way to change from string to PlayerID
    auto find = std::find(player_list.begin(), player_list.end(), id_to_kick);

    if (find == player_list.end()) {
        std::string text = "Player " + id_to_kick + " is not here.\n";
        msg_list = formMessageTo(text,player_id);
    } else if (player_id != admin) {
        std::string text = "Action Prohibited. You are not an admin.\n";
        msg_list = formMessageTo(text,player_id);
    } else {
        //leaveRoomCommand(id_to_kick);
        leaveRoomCommand(*find);
    }
    return msg_list;
}


std::vector<MessageReturn> GameManager::leaveRoomCommand(PlayerID player_id) {
    std::vector<MessageReturn> msg_list;
    std::vector<PlayerID> player_list;
    //player is not in a room
    if (playerid_to_roomid_map.count(player_id) == 0) {
        std::string text = "You are not in a room!\n";
        msg_list = formMessageTo(text,player_id);
    } else {
        RoomID curr_player_room_ID = playerid_to_roomid_map.at(player_id);
        roomid_to_room_map.at(curr_player_room_ID).playerLeave(player_id);
        std::string text = std::string(player_id) + " has left the room.\n";
        msg_list = formMessageToRoomMembers(text,player_id,false);
    }
    return msg_list;
}


std::vector<MessageReturn> GameManager::quitFromServerCommand(PlayerID player_id){

}


std::vector<MessageReturn> GameManager::shutdownServerCommand(PlayerID id){
    if(id != admin){
        std::string text = "Action Prohibited. You are not the admin of the server.\n";
        std::vector<MessageReturn> msg_list = formMessageTo(text,id);
        return msg_list;
    }
    else{
        std::string text = "Shutting down server...\n";
        std::vector<MessageReturn> msg_list = formMessageToEveryone(text,true); 
        return msg_list;
    }
}


std::vector<MessageReturn> GameManager::initRoomCommand(PlayerID id) {
}

*/

GameManager::ReturnCode
GameManager::destroyRoom(PlayerID player_id) {

    auto find_room_id = playerid_to_roomid_map.find(player_id);
    if (find_room_id == playerid_to_roomid_map.end()) return GameManager::ReturnCode::ROOM_NOT_EXIST;

    auto find_room = roomid_to_room_map.find(find_room_id->second);
    if (find_room == roomid_to_room_map.end()) return GameManager::ReturnCode::PLAYER_NOT_EXIST;

    if((find_room->second).getOwner() != player_id) return GameManager::ReturnCode::NO_PERMISSION;

    roomid_to_room_map.erase(find_room_id->second);

    for( auto it = playerid_to_roomid_map.begin(); it != playerid_to_roomid_map.end(); ) {
        if (it->second == find_room_id->second) {
            it = playerid_to_roomid_map.erase(it);
        } else {
            ++it;
        }
    }

    return GameManager::ReturnCode::SUCCESS;
    /*
    std::vector<MessageReturn> msg_list;
    std::vector<PlayerID> player_list;
    //player is not in room
    if (playerid_to_roomid_map.count(player_id) == 0) {
        std::string text = "Action Prohibited. You are not in a room.\n";
        //msg_list = formMessageTo(text,player_id);

    } else {
        RoomID room_to_destroy = playerid_to_roomid_map.at(player_id);
        Room player_room = roomid_to_room_map.at(room_to_destroy);
        int peopleInRoom = player_room.returnPlayers().size();
        //if room still has people in it
        if (peopleInRoom > 1) {
            std::string text = "Action Prohibited. There are still people in the room\n";
            //msg_list = formMessageTo(text,player_id);
        } else if (player_room.getOwner() != player_id) {
            std::string text = "Action Prohibited. You are not the owner of this room.\n";
            //msg_list = formMessageTo(text,player_id);
        } else {
            //TODO: clean up game instance when implemented
            roomid_to_room_map.erase(room_to_destroy);
            std::string text = "Room: " + std::string(player_id) + " has been removed.\n";
            //msg_list = formMessageTo(text,player_id);
        }
    }
    return msg_list;
    */
}

/*
std::vector<MessageReturn> GameManager::whisperCommand(PlayerID player_id, std::string recipient, std::string msg) {
    std::vector<MessageReturn> msg_list;
    std::vector<PlayerID> player_list;
    //TODO: find proper way to change from string to PlayerID
    if (username_to_playerid_map.count(recipient) == 0) {
        std::string text = "Player: " + std::string(player_id) + " does not exist.\n";
        msg_list = formMessageTo(text,player_id);
    } else {
      PlayerID recip_id = username_to_playerid_map.at(recipient);
        player_list.push_back(recip_id);
        std::string text = std::string(player_id) + " says: " + msg;
        for (auto& player : player_list) {
            msg_list.push_back(MessageReturn { player, text, false });
        }
    }
    return msg_list;
}
*/

std::optional<RoomID>
GameManager::getRoomIDOfPlayer(PlayerID player_id) {
    auto find_room_id = playerid_to_roomid_map.find(player_id);
    if (find_room_id == playerid_to_roomid_map.end()) return std::nullopt;
 
    return find_room_id->second;
}



Room& GameManager::getRoomFromPlayerID(PlayerID id){

    //auto room_id = playerid_to_roomid_map.at(id);
    //return roomid_to_room_map.at(room_id);
}


/*
std::vector<MessageReturn> GameManager::formMessageToRoomMembers(std::string& message, PlayerID& sentFrom, bool shouldShutdown){
    auto room = playerIDtoRoom(sentFrom);
    std::vector<PlayerID> members = room.returnPlayers();
    std::vector<MessageReturn> msg_list;
    for(auto member : members){
        msg_list.push_back(MessageReturn{member, message,shouldShutdown});
    }
    return msg_list;
}


std::vector<MessageReturn> GameManager::formMessageToEveryone(std::string& message, bool shouldShutdown){
    std::vector<MessageReturn> msg_list;
    for(auto member : all_players){
        msg_list.push_back(MessageReturn{member, message, false});
    }
    return msg_list;
}



std::vector<MessageReturn> GameManager::formMessageTo (std::string& message, PlayerID& recipient)  {
    return { MessageReturn{recipient,message,false} }; 
}


std::vector<MessageReturn> GameManager::formMessageTo (std::string& message, std::vector<PlayerID>& recipient)  {
    std::vector<MessageReturn> msg_list; 
    for(auto& id : recipient){
        msg_list.push_back(MessageReturn{id, message, false});
    }
    return msg_list;
}


//Issues command to client to clear chat history. Will be useful for initiating a game instance



std::vector<MessageReturn> GameManager::clearCommand(PlayerID playerId) {
    std::vector<MessageReturn> msg_list;
    std::string text = "/clear";
    msg_list = formMessageTo(text,playerId);
    return msg_list;
}

*/

GameManager::ReturnCode
GameManager::createRoom (PlayerID creator, RoomID room_id){
    roomid_to_room_map.insert({ room_id, Room(creator, room_id) });
    playerid_to_roomid_map.insert({ creator, room_id });

    if(roomid_to_room_map.count(room_id) == 1 && playerid_to_roomid_map.count(creator) == 1) {
        return GameManager::ReturnCode::SUCCESS;
    } else {
        return GameManager::ReturnCode::ROOM_NOT_EXIST;
    }

}


//Delegating addPlayerToRoom and removePlayerToRoom responsibilities solely to Room class...


GameManager::ReturnCode
GameManager::addPlayerToRoom (PlayerID player_id, RoomID room_id){

    auto find = roomid_to_room_map.find(room_id);
    if (find == roomid_to_room_map.end()) return GameManager::ReturnCode::ROOM_NOT_EXIST;

    if(true){  //satisfies all conditions eg. room capacity
        if(!roomid_to_room_map.at(room_id).addPlayer(player_id,gen_random_username)) return GameManager::ReturnCode::FAILURE;
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

    auto find_room = roomid_to_room_map.find(find_room_id->second);
    if (find_room == roomid_to_room_map.end()) return {std::nullopt, GameManager::ReturnCode::ROOM_NOT_EXIST};

    std::optional<std::string> username = (find_room->second).getUsernameFromPlayerID(player_id);

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

std::vector<PlayerID>& getPlayersInRoom(RoomID room_id) {
    


}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`

