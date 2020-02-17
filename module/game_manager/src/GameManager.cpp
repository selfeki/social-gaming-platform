
#include "GameManager.h"

#include <arepa/server/Server.h>

typedef std::string RoomID;
typedef networking::ConnectionId PlayerID;

using MessageReturn = GameManager::MessageReturn;


std::string gen_random(const int len) {
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


std::vector<PlayerID> Room::returnPlayers() {
    return players;
}


void Room::playerJoin(PlayerID player_id) {    //todo: should deal with some error handling
    /*
    Add player to the game, if DSL interpeter returns an error,
    do something (send back error message).
    */
    try {
        //game.addPlayer(player);
    } catch (... /* DSL_interpreter error */) {
    }

    players.push_back(player_id);
}


void Room::playerLeave(PlayerID player_id) {
    try {
        //game.playerQuit(player);
    } catch (... /* DSL_interpreter error */) {
    }

    players.erase(std::remove(players.begin(), players.end(), player_id));
}

//update the game's DSL interpreter

void Room::gameUpdate() {
}


PlayerID Room::getOwner() {
    return owner;
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


void GameManager::removePlayer(PlayerID player_id, RoomID room_id) {
}


void GameManager::addPlayer(PlayerID player_id, RoomID room_id) {
}


std::vector<PlayerID> GameManager::getPlayersInRoom(RoomID room_id) {
    std::vector<PlayerID> players;
}


std::vector<MessageReturn> GameManager::handleGameMessage(std::string msg, PlayerID player_id) {

    std::vector<MessageReturn> msg_list;
    //If player is not in a room, send this message
    if (playerid_to_roomid_map.count(player_id) == 0) {
        std::string text = "You are not in a room, please join or create a room.\n";
        msg_list = formMessageTo(text,player_id);
    }
    else {
        RoomID room_id = playerid_to_roomid_map.at(player_id);
        std::string text = "Room [" + room_id + "] " + std::string(player_id) + ": " + msg;
        msg_list = formMessageToRoomMembers(text, player_id, false);
    }
    return msg_list;
}


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


std::vector<MessageReturn> GameManager::destroyRoom(PlayerID player_id) {
    std::vector<MessageReturn> msg_list;
    std::vector<PlayerID> player_list;
    //player is not in room
    if (playerid_to_roomid_map.count(player_id) == 0) {
        std::string text = "Action Prohibited. You are not in a room.\n";
        msg_list = formMessageTo(text,player_id);

    } else {
        RoomID room_to_destroy = playerid_to_roomid_map.at(player_id);
        Room player_room = roomid_to_room_map.at(room_to_destroy);
        int peopleInRoom = player_room.returnPlayers().size();
        //if room still has people in it
        if (peopleInRoom > 1) {
            std::string text = "Action Prohibited. There are still people in the room\n";
            msg_list = formMessageTo(text,player_id);
        } else if (player_room.getOwner() != player_id) {
            std::string text = "Action Prohibited. You are not the owner of this room.\n";
            msg_list = formMessageTo(text,player_id);
        } else {
            //TODO: clean up game instance when implemented
            roomid_to_room_map.erase(room_to_destroy);
            std::string text = "Room: " + std::string(player_id) + " has been removed.\n";
            msg_list = formMessageTo(text,player_id);
        }
    }
    return msg_list;
}


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


Room GameManager::playerIDtoRoom(PlayerID& id){
    auto room_id = playerid_to_roomid_map.at(id);
    Room room = roomid_to_room_map.at(room_id);
    return room;
}


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


void GameManager::createRoom (PlayerID creator, RoomID room_id){
    roomid_to_room_map.insert({ room_id, Room(creator, room_id) });
    roomid_to_room_map.at(room_id).playerJoin(creator);
    playerid_to_roomid_map.insert({ creator, room_id });
}


void GameManager::addPlayerToRoom (PlayerID player_id, RoomID room_id){
    if(true /*satisfies all conditions eg. room capacity*/){
        Room room = roomid_to_room_map.at(room_id);
        room.playerJoin(player_id);
        playerid_to_roomid_map.insert({ player_id, room_id });
    }
}


void GameManager::removePlayerFromRoom (PlayerID player_id){
    if(true/*if action is allowed*/){
        RoomID room_id = playerid_to_roomid_map.at(player_id);
        Room room = roomid_to_room_map.at(room_id);
        room.playerLeave(player_id);
        playerid_to_roomid_map.erase(player_id);
    }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`


//template class GameManager<networking::ConnectionId>;

/*
*Avoids linking error allows us to use a template.
*/
