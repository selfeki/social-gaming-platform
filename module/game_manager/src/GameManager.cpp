#include "GameManager.hpp"

#include <arepa/Util.hpp>
#include <arepa/game/room/GameException.hpp>
#include <arepa/game/room/PlayerNicknameGenerator.hpp>
#include <arepa/game/room/interface/PlayerNetworking.hpp>
#include <arepa/server/Connection.hpp>

using arepa::game::room::GameException;
using arepa::game::room::PlayerNetworking;
using arepa::game::room::PlayerNicknameGenerator;
using arepa::game::room::Room;
using arepa::server::Connection;
using std::shared_ptr;

using Player = GameManager::Player;
using PlayerId = GameManager::PlayerId;
using Room = GameManager::Room;
using RoomId = GameManager::RoomId;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

GameManager::GameManager(const serverConfig::Configuration& config) {
    // TODO(ryan): Handle config
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Internal) -
// ---------------------------------------------------------------------------------------------------------------------

bool room_has_nickname(const std::shared_ptr<Room>& room, const Player::Name& name) {
    return room->find_spectator(name) || room->find_spectator(name);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Static) -
// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Player> GameManager::make_player(const shared_ptr<Connection>& connection) {
    std::shared_ptr<PlayerNetworking> net = connection;
    return std::make_shared<Player>(connection->session_id(), net);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Room> GameManager::create_room(const std::shared_ptr<Player>& creator) {
    constexpr size_t ROOM_CODE_LENGTH = 5;

    // Generate a random room code.
    // FIXME(any): This really should account for over 99999 rooms... but /shrug
    RoomId room_code = RoomId::generate(ROOM_CODE_LENGTH);
    while (this->_roomid_to_room.find(room_code) != this->_roomid_to_room.end()) {
        room_code = RoomId::generate(ROOM_CODE_LENGTH);
    }

    // Create the room object.
    auto room = std::make_pair(room_code, std::make_shared<Room>(room_code));
    this->_roomid_to_room.insert(room);

    // Add the player to the room.
    room.second->add_player(creator);
    this->player_join_room(creator, room.second);
    return room.second;
}

void GameManager::destroy_room(const std::shared_ptr<Room>& room) {
    // Remove all players.
    for (auto player : room->players()) {
        this->player_leave_room(player, room);
    }

    // Remove all spectators.
    for (auto player : room->spectators()) {
        this->player_leave_room(player, room);
    }

    // Remove the room.
    auto roomRef = this->_roomid_to_room.find(room->id());
    if (roomRef == this->_roomid_to_room.end()) {
        throw GameException(GameException::ROOM_NOT_FOUND);
    }

    this->_roomid_to_room.erase(roomRef);
}

void GameManager::player_leave_room(const std::shared_ptr<Player>& player, const std::shared_ptr<Room>& room) {
    room->remove_player(player->id());

    // Remove from the Player <-> Room map.
    auto find = this->_playerid_to_room.find(player->id());
    if (find != this->_playerid_to_room.end()) {
        this->_playerid_to_room.erase(find);
    }
}

void GameManager::player_join_room(const std::shared_ptr<Player>& player, const std::shared_ptr<Room>& room) {
    // Reset the player's nickname if need be.
    if (room_has_nickname(room, player->name())) {
        player->send_system_message("Your nickname is already used by somebody in the room you are joining.");
        player->clear_nickname();
    }

    // Generate a player nickname if need be.
    if (!player->nickname()) {
        auto nickname = PlayerNicknameGenerator::DEFAULT();
        player->send_system_message("Your new nickname is: " + *nickname);
        player->set_nickname(nickname);
    }

    // Add the player to the room.
    room->add_player(player);

    // Add to the Player <-> Room map.
    this->_playerid_to_room.emplace(player->id(), room);
}

std::optional<std::shared_ptr<Player>> GameManager::find_player(const PlayerId& player) {
    return arepa::find_in_map(this->_playerid_to_player, player);
}

std::optional<std::shared_ptr<Room>> GameManager::find_room(const RoomId& room) {
    return arepa::find_in_map(this->_roomid_to_room, room);
}

std::optional<std::shared_ptr<Room>> GameManager::find_player_room(const PlayerId& player) {
    return arepa::find_in_map(this->_playerid_to_room, player);
}

std::optional<std::shared_ptr<Room>> GameManager::find_player_room(const std::shared_ptr<Player>& player) {
    return this->find_player_room(player->id());
}

void GameManager::add_player(const std::shared_ptr<Player>& player) {
    this->_playerid_to_player.emplace(player->id(), player);
}

void GameManager::player_set_nickname(const std::shared_ptr<Player>& player, const Player::Name& name) {
    auto room = this->find_player_room(player);
    if (room) {
        if (room_has_nickname(*room, player->name())) {
            throw GameException(GameException::USER_NICKNAME_TAKEN);
        }

        if (player->is_playing()) {
            throw GameException(GameException::NOT_ALLOWED_DURING_GAME);
        }
    }

    player->set_nickname(name);
}
