
#include "GameManager.h"
#include "Server.h"

typedef std::string RoomID;
/*
*Custom exceptin handling class for the game manager, to give specific error messages about...
*game configuration errors, 
*administration errors
*other...
*/

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
    //for now this just sends a message into the chat room room
    if(player_room_map.count(player) == 0) {
        msg_list.push_back(messageReturn<IDType> {{player}, "You are not in a room, please join or create a room.", false});
    } else {
        RoomID room_id = player_room_map.at(player);
        std::cout << room_id << "\n";
        Room playersroom = id_room_map.at(room_id);
        std::string text = "Room [" + room_id + "] " + std::to_string(player) + ": " + msg;
        std::vector<IDType> player_list = playersroom.returnPlayers();
        msg_list.push_back(messageReturn<IDType> {player_list, text, false});
    }
    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::returnRoomMembersCommand(IDType id){

}
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::returnRoomCommand(IDType id){

}
template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::createRoomCommand(IDType player){
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list = {player};
    /*
    *generate unique key for a room. Check if any of the same value is already in map
    */
    RoomID room_code;

    //leave your previous room before creating new room...
    //leaveRoomCommand(id);


    do{
        room_code = gen_random(5);
    } while(id_room_map.count(room_code) > 0);

    id_room_map.insert({room_code, Room<IDType> (player, room_code) });
    id_room_map.at(room_code).playerJoin(player);
    player_room_map.insert({player, room_code});


    msg_list.push_back(messageReturn<IDType> {player_list, "Successfully created room "+room_code, false});

    return msg_list;
}

template <typename IDType>
std::vector<messageReturn<IDType>> GameManager<IDType>::joinRoomCommand(IDType id, std::string room_code){
    std::vector<messageReturn<IDType>> msg_list;
    std::vector<IDType> player_list;

    if(id_room_map.count(room_code) == 0) {
        player_list.push_back(id);
        std::string text = "room " + room_code + " does not exist...";
        msg_list.push_back(messageReturn<IDType> {player_list, text, false});
    } else {
        if(player_room_map.count(id) > 0) {
            if(player_room_map.at(id) == room_code) {
                player_list.push_back(id);
                std::string text = "You're alread in room " + room_code + "!";
                msg_list.push_back(messageReturn<IDType> {player_list, text, false});
            } else {
                //you would leave your previous room if you wrote a command to join another room, I guess
                leaveRoomCommand(id);
                //...
            }
        } else {
            id_room_map.at(room_code).playerJoin(id);
            player_list = id_room_map.at(room_code).returnPlayers();
            player_room_map.insert({id, room_code});
            std::string text = std::to_string(id) + " has joined room " + room_code + "!";
            msg_list.push_back(messageReturn<IDType> {player_list, text, false});
        }
    }
    return msg_list;
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

