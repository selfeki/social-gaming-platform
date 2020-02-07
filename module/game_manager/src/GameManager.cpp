
#include "GameManager.h"
#include "Server.h"

typedef std::string RoomID;

std::string gen_random(const int len) {
    std::string code = "";

    static const char alphanum[] =
        "0123456789"
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*
*Room class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

template <typename IDType>
Room<IDType>::Room(IDType _owner, RoomID room_code) : owner(_owner), room_id(room_code){
    //configRoomAndGame(game_config);
}

template <typename IDType>
void Room<IDType>::configRoomAndGame(json game_config) {

}

template <typename IDType>
std::vector<IDType> Room<IDType>::returnPlayers() {
    return players;
}

template <typename IDType>
void Room<IDType>::playerJoin(IDType player) { //todo: should deal with some error handling
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

    players.erase(std::remove(players.begin(), players.end(), player));
}

//update the game's DSL interpreter
template <typename IDType>
void Room<IDType>::gameUpdate() {
}

template <typename IDType>
IDType Room<IDType>::getOwner(){
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

    //If player is not in a room, send this message
    if(player_room_map.count(player) == 0) {
        msg_list.push_back(messageReturn<IDType> {{player}, "You are not in a room, please join or create a room.", false});
        return msg_list;
    }

    //else, get players room, and send their message to each player in that room
    RoomID room_id = player_room_map.at(player);
    //std::cout << room_id << "\n";
    Room playersroom = id_room_map.at(room_id);
    std::string text = "Room [" + room_id + "] " + std::to_string(player) + ": " + msg;
    std::vector<IDType> player_list = playersroom.returnPlayers();

    for(auto &player : player_list) {
        msg_list.push_back(messageReturn<IDType> {player, text, false});
    }

    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::returnRoomMembersCommand(IDType id){
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::returnRoomCommand(IDType id){
}

//Creates a new room.
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::createRoomCommand(IDType player){

    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list = {player};

    RoomID room_code;

    //Generate random key, while that key does not already exist in the map
    do{
        room_code = gen_random(5);
    } while(id_room_map.count(room_code) > 0);

    id_room_map.insert({room_code, Room<IDType> (player, room_code) });
    id_room_map.at(room_code).playerJoin(player);
    player_room_map.insert({player, room_code});

    for(auto &player : player_list) {
        msg_list.push_back(messageReturn<IDType> {player, "Successfully created room "+room_code, false});
    }

    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::joinRoomCommand(IDType id, std::string room_code){

    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;

    //if room does not exist, send error msg
    if(id_room_map.count(room_code) == 0) {
        player_list.push_back(id);
        std::string text = "room " + room_code + " does not exist...";
        for(auto &player : player_list) {
            msg_list.push_back(messageReturn<IDType> {player, text, false});
        }
        return msg_list;
    }

    if(player_room_map.count(id) > 0) {
        if(player_room_map.at(id) == room_code) {
            player_list.push_back(id);
            std::string text = "You're alread in room " + room_code + "!";
            for(auto &player : player_list) {
                msg_list.push_back(messageReturn<IDType> {player, text, false});
            }
        //if player is in a different room, leave that room and join this room
        } else {
            leaveRoomCommand(id);
            //...
        }
    //If player is not in a room, join this room, send message to every player in room that
    //a new player has joined
    } else {
        id_room_map.at(room_code).playerJoin(id);
        player_list = id_room_map.at(room_code).returnPlayers();
        player_room_map.insert({id, room_code});
        std::string text = std::to_string(id) + " has joined room " + room_code + "!";
        for(auto &player : player_list) {
            msg_list.push_back(messageReturn<IDType> {player, text, false});
        }
     }

    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::kickPlayerCommand(IDType id, std::string id_to_kick){
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;

    //TODO: find propper way to change from string to IDTYPE
    IDType player_to_kick = std::stoul(id_to_kick, nullptr, 0);

    if (id != admin){
        std::string msg_text = "Action Prohibited. You are not an admin.";
        player_list.push_back(id);
        for(auto &player : player_list){
            msg_list.push_back(messageReturn<IDType> {player, msg_text, false});
        }
    } else {
        //leaveRoomCommand(id_to_kick);
        leaveRoomCommand(player_to_kick);
    }
    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::leaveRoomCommand(IDType player_id){
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;
    //player is not in a room
    if(player_room_map.count(player_id) == 0){
        player_list.push_back(player_id);
        std::string text = "You are not in a room!";
        for(auto &player : player_list){
            msg_list.push_back(messageReturn<IDType> {player, text, false});
        }
    //player is in room
    } else {
        RoomID curr_player_room_ID = player_room_map.at(player_id);
        Room player_room = id_room_map.at(curr_player_room_ID);
        player_room.exitPlayer(player_id);
        std::string text = std::to_string(player_id) + " has left the room.";
        player_list = id_room_map.at(curr_player_room_ID).returnPlayers();
        //place user back into lobby
        for (auto &player : player_list){
            msg_list.push_back(messageReturn<IDType> {player, text, false});
        }
    }
    return msg_list;
}


template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::initRoomCommand(IDType id){
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::destroyRoom(IDType player_id){
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;
    //player is not in room
    if(player_room_map.count(player_id) == 0){
        std::string text = "Action Prohibited. You are not in a room.";
        for(auto &player : player_list){
            msg_list.push_back(messageReturn<IDType> {player, text, false});
        }
    }
    else{
        RoomID room_to_destroy = player_room_map.at(player_id);
        Room player_room = id_room_map.at(room_to_destroy);
        int peopleInRoom = player_room.returnPlayers().size();
        //if room still has people in it
        if(peopleInRoom > 1){
            std::string text = "Action Prohibited. There are still people in the room";
            for(auto &player : player_list){
                msg_list.push_back(messageReturn<IDType> {player, text, false});
            }
        } else if (player_room.getOwner() != player_id){
            std::string text = "Action Prohibited. You are not the owner of this room.";
            for(auto &player : player_list){
                msg_list.push_back(messageReturn<IDType> {player, text, false});
            }
        } else {
            //TODO: clean up game instance when implemented
            id_room_map.erase(room_to_destroy);
            std::string text = "Room: " + std::to_string(player_id) + " has been removed.";
            for(auto &player : player_list){
                msg_list.push_back(messageReturn<IDType> {player, text, false});
            }
        }
    }
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::whisperCommand(IDType player_id, std::string recipient_id, std::string msg){
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;
    //TODO: find propper way to change from string to IDTYPE
    IDType recip_of_wsp = std::stoul(recipient_id, nullptr, 0);

    if(std::find(all_players.begin(), all_players.end(), recip_of_wsp)!= all_players.end()){
        std::string text = "Player: " + std::to_string(player_id) + " does not exist.";
        for(auto &player : player_list){
            msg_list.push_back(messageReturn<IDType> {player, text, false});
        }
    } else {
        player_list.push_back(recip_of_wsp);
        std::string text = id_player_map.at(recip_of_wsp) + " says: " + msg;
        for(auto &player : player_list){
            msg_list.push_back(messageReturn<IDType> {player, text, false});
        }
    }
    return msg_list;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`


template class GameManager<uintptr_t>;

/*
*Avoids linking error allows us to use a template.
*/
