#include "GameManager.hpp"

#include "GameUser.hpp"

#include <arepa/Util.hpp>
#include <arepa/game/GameException.hpp>
#include <arepa/game/UserNicknameGenerator.hpp>
#include <arepa/server/Connection.hpp>

using arepa::game::GameException;
using arepa::game::UserNicknameGenerator;
using arepa::server::Connection;
using std::shared_ptr;

using User = GameManager::User;
using UserId = GameManager::User;
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

bool room_has_nickname(const std::shared_ptr<Room>& room, const User::Name& name) {
    return room->find_spectator(name) || room->find_spectator(name);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Static) -
// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<User> GameManager::make_user(const shared_ptr<Connection>& connection) {
    return std::make_shared<GameUser>(connection);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Room> GameManager::create_room() {
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
    return room.second;
}

void GameManager::destroy_room(const std::shared_ptr<Room>& room) {
    // Remove all players.
    std::set<std::shared_ptr<User>> users;
    for (const auto& user : users) {
        room->remove_user(user);
    }

    // Remove the room.
    auto roomRef = this->_roomid_to_room.find(room->id());
    if (roomRef == this->_roomid_to_room.end()) {
        throw GameException(GameException::ROOM_NOT_FOUND);
    }

    this->_roomid_to_room.erase(roomRef);
}

void GameManager::user_leave_room(const std::shared_ptr<User>& user, const std::shared_ptr<Room>& room) {
    room->remove_user(user);

    // Remove from the User <-> Room map.
    auto find = this->_userid_to_room.find(user->id());
    if (find != this->_userid_to_room.end()) {
        this->_userid_to_room.erase(find);
    }

    // Remove the room (if empty).
    if (room->players().empty() /* && room->spectators().empty() */) {
        this->destroy_room(room);
    }
}

void GameManager::user_join_room(const std::shared_ptr<User>& user, const std::shared_ptr<Room>& room) {
    if (this->find_user_room(user)) {
        user->send_system_message("Error: You must leave your current room before joining another one.");
        return;
    }

    // Reset the user's nickname if need be.
    if (room_has_nickname(room, user->name())) {
        user->send_system_message("Your nickname is already used by somebody in the room you are joining.");
        user->clear_nickname();
    }

    // Generate a user nickname if need be.
    if (!user->nickname()) {
        auto nickname = UserNicknameGenerator::DEFAULT();
        user->send_system_message("Your new nickname is: " + *nickname);
        user->set_nickname(nickname);
    }

    // Add the user to the room.
    room->add_user(user);

    // Add to the User <-> Room map.
    this->_userid_to_room.emplace(user->id(), room);
}

std::shared_ptr<User> GameManager::find_user(const UserId& user) {
    auto find = arepa::find_in_map(this->_userid_to_user, user);
    return find ? *find : nullptr;
}

std::shared_ptr<Room> GameManager::find_room(const RoomId& room) {
    auto find = arepa::find_in_map(this->_roomid_to_room, room);
    return find ? *find : nullptr;
}

std::shared_ptr<Room> GameManager::find_user_room(const UserId& user) {
    auto find = arepa::find_in_map(this->_userid_to_room, user);
    return find ? *find : nullptr;
}

std::shared_ptr<Room> GameManager::find_user_room(const std::shared_ptr<User>& user) {
    return this->find_user_room(user->id());
}

void GameManager::add_user(const std::shared_ptr<User>& user) {
    this->_userid_to_user.emplace(user->id(), user);
}

void GameManager::remove_user(const std::shared_ptr<User>& user) {
    auto find = this->_userid_to_user.find(user->id());
    if (find == this->_userid_to_user.end()) {
        // TODO(ethan): Warning about removing a user when they aren't in the list.
        return;
    }

    // Remove the user from their room.
    auto room = this->find_user_room(find->second);
    if (room) {
        this->user_leave_room(find->second, room);
    }
}

void GameManager::user_set_nickname(const std::shared_ptr<User>& user, const User::Name& name) {
    auto room = this->find_user_room(user);
    if (room) {
        if (room_has_nickname(room, user->name())) {
            throw GameException(GameException::USER_NICKNAME_TAKEN);
        }

        if (room->is_game_active() && room->is_player(user)) {
            throw GameException(GameException::NOT_ALLOWED_DURING_GAME);
        }

        room->broadcast_message("[~] " + std::string(user->name()) + " is now known as " + std::string(name) + ".");
    }

    user->set_nickname(name);
}
