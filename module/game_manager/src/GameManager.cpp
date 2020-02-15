
#include "GameManager.h"

#include <arepa/server/Server.h>

typedef std::string RoomID;

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

template <typename IDType>
Room<IDType>::Room(IDType _owner, RoomID room_code)
    : owner(_owner)
    , room_id(room_code) {
    //configRoomAndGame(game_config);
}

template <typename IDType>
void Room<IDType>::configRoomAndGame(const g_config& game_config) {
}

template <typename IDType>
std::vector<IDType> Room<IDType>::returnPlayers() {
    return players;
}

template <typename IDType>
void Room<IDType>::playerJoin(IDType player) {    //todo: should deal with some error handling
    /*
    Add player to the game, if DSL interpeter returns an error,
    do something (send back error message).
    */
    try {
        //game.addPlayer(player);
    } catch (... /* DSL_interpreter error */) {
    }

    players.push_back(player);
}

template <typename IDType>
void Room<IDType>::exitPlayer(IDType player) {
    try {
        //game.playerQuit(player);
    } catch (... /* DSL_interpreter error */) {
    }

    players.erase(std::remove(players.begin(), players.end(), player));
}

//update the game's DSL interpreter
template <typename IDType>
void Room<IDType>::gameUpdate() {
}

template <typename IDType>
IDType Room<IDType>::getOwner() {
    return owner;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*
*GameManager class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

template <typename IDType>
GameManager<IDType>::GameManager() {
    //commands = commandSpace::Command<IDType>();
}

template <typename IDType>
void GameManager<IDType>::setUp(const s_config& server_config) {
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
std::vector<messageReturn<IDType>> GameManager<IDType>::handleGameMessage(std::string msg, IDType playerId) {

    std::vector<messageReturn<IDType>> msg_list;
    //If player is not in a room, send this message
    if (player_room_map.count(playerId) == 0) {
        std::string text = "You are not in a room, please join or create a room.\n";
        msg_list = formMessageTo(text,playerId);
    }
    else {
        RoomID room_id = player_room_map.at(playerId);
        std::string text = "Room [" + room_id + "] " + std::string(playerId) + ": " + msg;
        msg_list = formMessageToRoomMembers(text, playerId, false);
    }
    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::returnRoomMembersCommand(IDType player_id){
    std::vector<messageReturn<IDType>> msg_list;

    RoomID player_room_id = player_room_map.at(player_id);
    if (id_room_map.count(player_room_id) == 0) {
        std::string text = "Room " + player_room_id + " does not exist.\n";
        msg_list = formMessageTo(text,player_id);
    } else{
        Room player_room = id_room_map.at(player_room_id);
        std::vector<IDType> players_in_room = player_room.returnPlayers();
        std::string text = "";
        for(auto& player : players_in_room){
            text = text + std::string(player) + "\n";
        }
        msg_list = formMessageTo(text,player_id);
    }
    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::returnRoomCommand(IDType player_id) {
    std::vector<messageReturn<IDType>> msg_list;

    if (player_room_map.count(player_id) == 0) {
        std::string text = "You are not in a room.\n";
        msg_list = formMessageTo(text,player_id); 
    } else {
        RoomID player_room_id = player_room_map.at(player_id);
        std::string text = "Room ID: " + std::string();
        msg_list = formMessageTo(text,player_id); 
    }
    return msg_list;
}

//Creates a new room.
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::createRoomCommand(IDType playerId) {

    std::vector<messageReturn<IDType>> msg_list;
    RoomID room_code;

    //Generate random key, while that key does not already exist in the map
    do {
        room_code = gen_random(5);
    } while (id_room_map.count(room_code) > 0);

    createRoom(playerId, room_code);

    std::string text = "Successfully created room " + room_code + "\n";
    msg_list = formMessageTo(text,playerId);

    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::joinRoomCommand(IDType id, std::string room_code) {

    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;

    //if room does not exist, send error msg
    if (id_room_map.count(room_code) == 0) {
        player_list.push_back(id);
        std::string text = "room " + room_code + " does not exist...\n";
        msg_list = formMessageTo(text, id);
    }else if (player_room_map.count(id) > 0) {
        if (player_room_map.at(id) == room_code) {
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
        id_room_map.at(room_code).playerJoin(id);
        player_room_map.insert({ id, room_code });
        std::string text = string(id) + " has joined room " + room_code + "!\n";
        msg_list = formMessageToRoomMembers(text,id,false);
    }

    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::kickPlayerCommand(IDType id, std::string id_to_kick) {
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;

    //TODO: find propper way to change from string to IDTYPE
    auto find = std::find(player_list.begin(), player_list.end(), id_to_kick);

    if (find == player_list.end()) {
        std::string text = "Player " + id_to_kick + " is not here.\n";
        msg_list = formMessageTo(text,id);
    } else if (id != admin) {
        std::string text = "Action Prohibited. You are not an admin.\n";
        msg_list = formMessageTo(text,id);
    } else {
        //leaveRoomCommand(id_to_kick);
        leaveRoomCommand(*find);
    }
    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::leaveRoomCommand(IDType player_id) {
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;
    //player is not in a room
    if (player_room_map.count(player_id) == 0) {
        std::string text = "You are not in a room!\n";
        msg_list = formMessageTo(text,player_id);
    } else {
        RoomID curr_player_room_ID = player_room_map.at(player_id);
        id_room_map.at(curr_player_room_ID).exitPlayer(player_id);
        std::string text = std::string(player_id) + " has left the room.\n";
        msg_list = formMessageToRoomMembers(text,player_id,false);
    }
    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::quitFromServerCommand(IDType player_id){

}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::shutdownServerCommand(IDType id){
    if(id != admin){
        std::string text = "Action Prohibited. You are not the admin of the server.\n";
        std::vector<messageReturn<IDType>> msg_list = formMessageTo(text,id);
        return msg_list;
    }
    else{
        std::string text = "Shutting down server...\n";
        std::vector<messageReturn<IDType>> msg_list = formMessageToEveryone(text,true); 
        return msg_list;
    }
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::initRoomCommand(IDType id) {
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::destroyRoom(IDType player_id) {
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;
    //player is not in room
    if (player_room_map.count(player_id) == 0) {
        std::string text = "Action Prohibited. You are not in a room.\n";
        msg_list = formMessageTo(text,player_id);

    } else {
        RoomID room_to_destroy = player_room_map.at(player_id);
        Room player_room = id_room_map.at(room_to_destroy);
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
            id_room_map.erase(room_to_destroy);
            std::string text = "Room: " + std::string(player_id) + " has been removed.\n";
            msg_list = formMessageTo(text,player_id);
        }
    }
    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::whisperCommand(IDType player_id, std::string recipient, std::string msg) {
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;
    //TODO: find propper way to change from string to IDTYPE
    if (userName_id_map.count(recipient) == 0) {
        std::string text = "Player: " + std::string(player_id) + " does not exist.\n";
        msg_list = formMessageTo(text,player_id);
    } else {
      IDType recip_id = userName_id_map.at(recipient);
        player_list.push_back(recip_id);
        std::string text = std::string(player_id) + " says: " + msg;
        for (auto& player : player_list) {
            msg_list.push_back(messageReturn<IDType> { player, text, false });
        }
    }
    return msg_list;
}

template <typename IDType>
Room<IDType> GameManager<IDType>::playerIDtoRoom(IDType& id){
    auto room_id = player_room_map.at(id);
    Room<IDType> room = id_room_map.at(room_id);
    return room;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::formMessageToRoomMembers(std::string& message, IDType& sentFrom, bool shouldShutdown){
    auto room = playerIDtoRoom(sentFrom);
    std::vector<IDType> members = room.returnPlayers();
    std::vector<messageReturn<IDType>> msg_list;
    for(auto member : members){
        msg_list.push_back(messageReturn<IDType>{member, message,shouldShutdown});
    }
    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::formMessageToEveryone(std::string& message, bool shouldShutdown){
    std::vector<messageReturn<IDType>> msg_list;
    for(auto member : all_players){
        msg_list.push_back(messageReturn<IDType>{member, message, false});
    }
    return msg_list;
}


template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::formMessageTo (std::string& message, IDType& recipient)  {
    std::vector<messageReturn<IDType>> msg_list = { messageReturn<IDType>{recipient,message,false} };
    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::formMessageTo (std::string& message, std::vector<IDType>& recipient)  {
    std::vector<messageReturn<IDType>> msg_list; 
    for(auto& id : recipient){
        msg_list.push_back(messageReturn<IDType>{id, message, false});
    }
    return msg_list;
}
//Issues command to client to clear chat history. Will be useful for initiating a game instance
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::clearCommand(IDType playerId) {
    std::vector<messageReturn<IDType>> msg_list;
    std::string text = "/clear";
    msg_list = formMessageTo(text,playerId);
    return msg_list;
}

template <typename IDType>
void GameManager<IDType>::createRoom (IDType creator, RoomID room_id){
    id_room_map.insert({ room_id, Room<IDType>(creator, room_id) });
    id_room_map.at(room_id).playerJoin(creator);
    player_room_map.insert({ creator, room_id });
}

template <typename IDType>
void GameManager<IDType>::addPlayerToRoom (IDType player_id, RoomID room_id){
    if(true /*satisfies all conditions eg. room capacity*/){
        Room<IDType> room = id_room_map.at(room_id);
        room.playerJoin(player_id);
        player_room_map.insert({ player_id, room_id });
    }
}

template <typename IDType>
void GameManager<IDType>::removePlayerFromRoom (IDType player_id){
    if(true/*if action is allowed*/){
        RoomID room_id = player_room_map.at(player_id);
        Room<IDType> room = id_room_map.at(room_id);
        room.exitPlayer(player_id);
        player_room_map.erase(player_id);
    }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`


template class GameManager<networking::ConnectionId>;

/*
*Avoids linking error allows us to use a template.
*/
