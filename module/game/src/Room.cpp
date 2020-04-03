#include "Room.hpp"

#include "GameException.hpp"

#include <arepa/Util.hpp>

#include <algorithm>

using namespace arepa::game;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - UserContainer -
// ---------------------------------------------------------------------------------------------------------------------

Room::UserContainer::UserContainer(std::shared_ptr<User> user, arepa::game::Room::UserStatus status)
    : joined_at(std::chrono::system_clock::now())
    , status(status)
    , user(std::move(user)) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - UserContainerGuard -
// ---------------------------------------------------------------------------------------------------------------------

Room::UserContainerGuard::UserContainerGuard(arepa::game::Room* room, arepa::game::Room::UserContainer& container)
    : _room(*room)
    , _container(container) {
    this->_room._cached_remove(this->_container);
}

Room::UserContainerGuard::~UserContainerGuard() {
    this->_room._cached_add(this->_container);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Room::Room(Room::Id id)
    : _created_at(std::chrono::system_clock::now())
    , _id(std::move(id))
    , _player_limit(8) {}

Room::Room()
    : Room(Room::Id::generate(5)) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Private) -
// ---------------------------------------------------------------------------------------------------------------------

const Room::UserContainer* Room::_find_user_container(const Room::User::Id& id) const {
    auto find = this->_users.find(id);
    if (find == this->_users.end()) {
        return nullptr;
    }

    return &(find->second);
}

void Room::_cached_remove(const arepa::game::Room::UserContainer& container) {
    this->_cached_users.erase(container.user);

    if (this->is_spectator(container)) {
        this->_cached_spectators.erase(container.user);
    }

    if (this->is_player(container)) {
        this->_cached_players.erase(container.user);
    }
}

void Room::_cached_add(const arepa::game::Room::UserContainer& container) {
    this->_cached_users.insert(container.user);

    if (this->is_spectator(container)) {
        this->_cached_spectators.insert(container.user);
    }

    if (this->is_player(container)) {
        this->_cached_players.insert(container.user);
    }
}

const Room::UserContainer* Room::_find_user_container(const Room::User::Name& name) const {
    for (const auto& entry : this->_users) {
        if (entry.second.user->name() == name) {
            return &entry.second;
        }
    }

    return nullptr;
}

const Room::UserContainer* Room::_find_user_container(const std::shared_ptr<game::User>& user) const {
    return this->_find_user_container(user->id());
}

Room::UserContainer* Room::_find_user_container(const Room::User::Id& id) {
    return const_cast<Room::UserContainer*>(std::as_const(*this)._find_user_container(id));
}

Room::UserContainer* Room::_find_user_container(const Room::User::Name& name) {
    return const_cast<Room::UserContainer*>(std::as_const(*this)._find_user_container(name));
}

Room::UserContainer* Room::_find_user_container(const std::shared_ptr<game::User>& user) {
    return const_cast<Room::UserContainer*>(std::as_const(*this)._find_user_container(user));
}

bool Room::is_spectator(const UserContainer& user) const {
    return user.status == Room::UserStatus::SPECTATOR
        || user.status == Room::UserStatus::PLAYER_WAITLIST;
}

bool Room::is_spectating(const UserContainer& user) const {
    return this->is_game_active() && this->is_spectator(user);
}

bool Room::is_player(const UserContainer& user) const {
    return user.status == Room::UserStatus::PLAYER;
}

bool Room::is_waitlisted(const UserContainer& user) const {
    return user.status == Room::UserStatus::PLAYER_WAITLIST;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

const Room::Id& Room::id() const {
    return this->_id;
}

std::shared_ptr<Room::User> Room::owner() const {
    return this->_owner;
}

bool Room::is_full() const {
    return this->player_count() >= this->player_limit();
}

const std::set<std::shared_ptr<Room::User>>& Room::users() const {
    return this->_cached_users;
}

const std::set<std::shared_ptr<Player>>& Room::players() const {
    return this->_cached_players;
}

const std::set<std::shared_ptr<Spectator>>& Room::spectators() const {
    return this->_cached_spectators;
}

const std::chrono::system_clock::time_point& Room::creation_date() const {
    return this->_created_at;
}

size_t Room::player_limit() const {
    return this->_player_limit;
}

size_t Room::player_count() const {
    return this->_cached_players.size();
}

size_t Room::spectator_limit() const {
    return this->_spectator_limit;
}

size_t Room::spectator_count() const {
    return this->_cached_players.size();
}

size_t Room::user_count() const {
    return this->_users.size();
}

void Room::remove_user(const std::shared_ptr<User>& user) {
    auto container = this->_find_user_container(user);
    if (!container) {
        // The user isn't in the room.
        // TODO(ethan): Log a warning.
        return;
    }

    // Remove the user from the cached lists.
    this->_cached_remove(*container);

    // Remove the user from the users map.
    this->_users.erase(user->id());

    // Shift the owner to the next person.
    if (user == this->owner()) {
        if (this->_users.empty()) {
            this->_owner = nullptr;
        } else {
            auto candidates = this->_users_by_age<std::greater<std::chrono::system_clock::time_point>>([this](const UserContainer& container) {
                return this->is_player(container);
            });

            this->set_owner(candidates.front()->user);
        }
    }
}

void Room::add_user(const std::shared_ptr<User>& user, Room::Join method) {
    size_t current_players = this->player_count() - (this->is_player(user) ? 1 : 0);
    size_t current_spectators = this->spectator_count() - (this->is_spectator(user) ? 1 : 0);

    UserStatus status;
    switch (method) {
    case Room::Join::AS_PLAYER:

        if (current_players >= this->player_limit()) {
            throw GameException(GameException::ROOM_FULL);
        }

        if (this->is_game_active()) {
            throw GameException(GameException::NOT_ALLOWED_DURING_GAME);
        }

        status = UserStatus::PLAYER;
        break;

    case Room::Join::AS_SPECTATOR:
        if (current_spectators < this->spectator_limit()) {
            status = UserStatus::SPECTATOR;
            break;
        }

        throw GameException(GameException::ROOM_FULL);

    case Room::Join::AS_PLAYER_WAITING:
        if (current_players < this->player_limit()) {
            status = UserStatus::PLAYER;
            break;
        } else if (current_spectators < this->spectator_limit()) {
            status = UserStatus::SPECTATOR;
            break;
        }

        throw GameException(GameException::ROOM_FULL);
    }

    // Update the existing user container, or add a new user container.
    Room::UserContainer* container = this->_find_user_container(user);
    if (container) {
        Room::UserContainerGuard guard(this, *container);
        container->status = status;
    } else {
        this->_users.insert({ user->id(), Room::UserContainer(user, status) });
        this->_cached_add(*this->_find_user_container(user));

        // If there isn't an owner, make the new user an owner.
        if (!this->_owner) {
            this->set_owner(user);
        }
    }
}

bool Room::is_game_active() const {
    // TODO(ethan): Game instance.
    return false;
}

void Room::update_game() {
    // TODO(ethan): Game instance.
}

void Room::set_owner(std::shared_ptr<AbstractUser> owner) {
    this->_owner = std::move(owner);
}

void Room::set_player_limit(size_t limit) {
    if (limit < 1) {
        throw GameException(GameException::NOT_ALLOWED_OPTION, "Player limit must be positive.");
    }

    ssize_t overflow = this->user_count() - limit;
    if (overflow > 0) {
        std::vector<UserContainer*> remove = this->_users_by_age(overflow, [this](const auto& container) {
            return this->is_player(container);
        });

        // Perform the removal.
        for (auto& user : remove) {
            Room::UserContainerGuard guard(this, *user);
            user->status = UserStatus::PLAYER_WAITLIST;
        }
    }
}

void Room::set_spectator_limit(size_t limit) {
    ssize_t overflow = this->spectator_count() - limit;

    if (overflow > 0) {
        std::vector<UserContainer*> remove;
        for (auto& pair : this->_users) {
            if (this->is_player(pair.second)) {
                remove.push_back(&pair.second);
            }
        }

        // Sort the spectators.
        // Order:
        // - Not waitlisted.
        // - Most recently joined.
        std::sort(remove.begin(), remove.end(), [this](const auto& a, const auto& b) {
            if (this->is_waitlisted(*a) && !this->is_waitlisted(*b)) {
                return true;
            }

            return a->joined_at > b->joined_at;
        });

        // Trim the vector.
        remove.erase(remove.begin() += overflow, remove.end());

        // Perform the removal.
        for (const auto& user : remove) {
            this->remove_user(user->user);
        }
    }
}

std::shared_ptr<Player> Room::find_player(const std::shared_ptr<User>& player) const {
    return this->find_player(player->id());
}

std::shared_ptr<Player> Room::find_player(Room::User::Id player) const {
    auto find = std::find_if(this->_cached_players.begin(), this->_cached_players.end(), [&player](const auto& other) {
        return other->id() == player;
    });

    return find == this->_cached_players.end() ? nullptr : *find;
}

std::shared_ptr<Player> Room::find_player(Room::User::Name player) const {
    auto find = std::find_if(this->_cached_players.begin(), this->_cached_players.end(), [&player](const auto& other) {
        return other->name() == player;
    });

    return find == this->_cached_players.end() ? nullptr : *find;
}

std::shared_ptr<Spectator> Room::find_spectator(const std::shared_ptr<User>& spectator) const {
    return this->find_spectator(spectator->id());
}

std::shared_ptr<Spectator> Room::find_spectator(Room::User::Id spectator) const {
    auto find = std::find_if(this->_cached_spectators.begin(), this->_cached_spectators.end(), [&spectator](const auto& other) {
        return other->id() == spectator;
    });

    return find == this->_cached_spectators.end() ? nullptr : *find;
}

std::shared_ptr<Spectator> Room::find_spectator(Room::User::Name spectator) const {
    auto find = std::find_if(this->_cached_spectators.begin(), this->_cached_spectators.end(), [&spectator](const auto& other) {
        return other->name() == spectator;
    });

    return find == this->_cached_spectators.end() ? nullptr : *find;
}

std::shared_ptr<Room::User> Room::find_user(Room::User::Id user) const {
    auto find = std::find_if(this->_cached_users.begin(), this->_cached_users.end(), [&user](const auto& other) {
        return other->id() == user;
    });

    return find == this->_cached_users.end() ? nullptr : *find;
}

std::shared_ptr<Room::User> Room::find_user(Room::User::Name user) const {
    auto find = std::find_if(this->_cached_users.begin(), this->_cached_users.end(), [&user](const auto& other) {
        return other->name() == user;
    });

    return find == this->_cached_users.end() ? nullptr : *find;
}

bool Room::is_spectator(const std::shared_ptr<Room::User>& user) const {
    auto find = this->_find_user_container(user);
    return find && this->is_spectator(*find);
}

bool Room::is_spectating(const std::shared_ptr<Room::User>& user) const {
    auto find = this->_find_user_container(user);
    return find && this->is_spectating(*find);
}

bool Room::is_player(const std::shared_ptr<Room::User>& user) const {
    auto find = this->_find_user_container(user);
    return find && this->is_player(*find);
}

void Room::broadcast_message(const std::string& message) {
    for (auto& user : this->_cached_users) {
        user->send_message(message);
    }
}

void Room::broadcast_message_to_spectators(const std::string& message) {
    for (auto& user : this->_cached_spectators) {
        user->send_message(message);
    }
}

void Room::broadcast_packet(const User::Packet& packet) {
    for (auto& user : this->_cached_users) {
        user->send_packet(packet);
    }
}

bool Room::process_command(Room::User::Id user, const arepa::command::Command& command) {
    // Get the user container.
    auto container = this->_find_user_container(user);
    if (!container) {
        // TODO(ethan): Log the warning.
        return false;
    }

    // Try processing it through the game instance.
    if (this->is_game_active() && this->is_player(*container)) {
        // TODO(ethan): Try game instance command.
    }

    // Get the executor for the command.
    auto executor = this->commands.find(command.name());
    if (!executor) {
        return false;
    }

    // Execute the command.
    (*executor)->execute(*this, *container->user, command.arguments());
    return true;
}

bool Room::process_message(Room::User::Id user, const std::string& message) {
    // Get the player object.
    auto container = this->_find_user_container(user);
    if (!container) {
        // TODO(ethan): Log the warning.
        return false;
    }

    // Try processing it through the game instance.
    if (this->is_game_active() && this->is_player(*container)) {
        // TODO(ethan): Try game instance messages.
    }

    // Create a regular player message.
    std::string messagePrefix = this->is_spectating(*container) ? "[spectator] " : "";
    std::string messageSender = container->user->name();
    std::string outbound = messagePrefix + messageSender + ": " + message;

    // Handle it normally.
    this->broadcast_message(outbound);
    return true;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Operators -
// ---------------------------------------------------------------------------------------------------------------------

bool Room::operator==(const Room& other) const {
    return this->_id == other._id;
}

bool Room::operator!=(const Room& other) const {
    return !(*this == other);
}
