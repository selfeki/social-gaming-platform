#include "ServerManager.hpp"

#include "ServerUser.hpp"

#include <arepa/Util.hpp>
#include <arepa/chat/ChatException.hpp>
#include <arepa/chat/UserNicknameGenerator.hpp>
#include <arepa/server/Connection.hpp>

using arepa::chat::ChatException;
using arepa::chat::MemberPtr;
using arepa::chat::UserNicknameGenerator;
using arepa::server::Connection;
using std::shared_ptr;

using namespace arepa::server;

using User = ServerManager::User;
using UserId = ServerManager::User;
using Room = ServerManager::Room;
using RoomId = ServerManager::RoomId;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

ServerManager::ServerManager(const serverConfig::Configuration& config) {
    // TODO(ryan): Handle config
}

ServerManager::~ServerManager() {
    for (auto& pair : this->_roomid_to_room) {
        auto room = pair.second;
        this->destroy_room(room);
    }
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Static) -
// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<User> ServerManager::make_user(const shared_ptr<Connection>& connection) {
    return std::make_shared<ServerUser>(connection);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

shared_ptr<Room> ServerManager::create_room() {
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

void ServerManager::destroy_room(const std::shared_ptr<Room>& room) {
    // Remove all players.
    std::set<std::shared_ptr<User>> users;
    for (const auto& user : users) {
        room->remove_user(room->find_player(user));
    }

    // Remove the room.
    auto roomRef = this->_roomid_to_room.find(room->id());
    if (roomRef == this->_roomid_to_room.end()) {
        throw ChatException(ChatException::ROOM_NOT_FOUND);
    }

    this->_roomid_to_room.erase(roomRef);
}

void ServerManager::user_leave_room(const std::shared_ptr<User>& user, const std::shared_ptr<Room>& room) {
    // Remove from the User <-> Room map.
    auto find = this->_userid_to_room.find(user->id());
    if (find != this->_userid_to_room.end()) {
        this->_userid_to_room.erase(find);
    }

    // Remove from the room.
    try {
        room->remove_user(room->find_player(user));
    } catch (ChatException& ex) {
        // They weren't in the room. Do nothing.
    }

    // Remove the room (if empty).
    if (room->players().empty() /* && room->spectators().empty() */) {
        this->destroy_room(room);
    }
}

void ServerManager::user_join_room(const std::shared_ptr<User>& user, const std::shared_ptr<Room>& room) {
    if (this->find_user_room(user)) {
        user->send_system_message("Error: You must leave your current room before joining another one.");
        return;
    }

    // Reset the user's nickname if need be.
    if (room->find_user(user->name())) {
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

std::shared_ptr<User> ServerManager::find_user(const UserId& user) {
    auto find = arepa::find_in_map(this->_userid_to_user, user);
    return find ? *find : nullptr;
}

std::shared_ptr<Room> ServerManager::find_room(const RoomId& room) {
    auto find = arepa::find_in_map(this->_roomid_to_room, room);
    return find ? *find : nullptr;
}

std::shared_ptr<Room> ServerManager::find_user_room(const UserId& user) {
    auto find = arepa::find_in_map(this->_userid_to_room, user);
    return find ? *find : nullptr;
}

std::shared_ptr<Room> ServerManager::find_user_room(const std::shared_ptr<User>& user) {
    return this->find_user_room(user->id());
}

void ServerManager::add_user(const std::shared_ptr<User>& user) {
    this->_userid_to_user.emplace(user->id(), user);
}

void ServerManager::remove_user(const std::shared_ptr<User>& user) {
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

void ServerManager::user_set_nickname(const std::shared_ptr<User>& user, const User::Name& name) {
    auto room = this->find_user_room(user);
    if (room) {
        if (room->find_user(name)) {
            throw ChatException(ChatException::USER_NICKNAME_TAKEN);
        }

        if (room->is_game_active() && room->find_player(user)) {
            throw ChatException(ChatException::NOT_ALLOWED_DURING_GAME);
        }

        room->broadcast_message("[~] " + std::string(user->name()) + " is now known as " + std::string(name) + ".");
    }

    user->set_nickname(name);
}
