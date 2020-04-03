#include "Room.hpp"

#include "GameException.hpp"

#include <arepa/Util.hpp>

using namespace arepa::game;

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

void Room::_join_spectator() {
    auto next = std::find_if(this->_spectators.begin(), this->_spectators.end(), [](const std::shared_ptr<User>& spec) {
        return spec->_status == User::Status::SPECTATOR_WAITLIST;
    });

    if (next != this->_spectators.end()) {
        this->_spectators.erase(next);
        this->add_player((*next));
    }
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

const Room::Id& Room::id() const {
    return this->_id;
}

std::optional<std::shared_ptr<User>> Room::owner() const {
    return this->_owner;
}

bool Room::is_full() const {
    return this->_players.size() >= this->player_limit();
}

const std::vector<std::shared_ptr<User>>& Room::players() const {
    return this->_players;
}

const std::vector<std::shared_ptr<User>>& Room::players_in_game() const {
    // TODO(ethan): When playing a game.
    return this->players();
}

const std::vector<std::shared_ptr<User>>& Room::spectators() const {
    return this->_spectators;
}

const std::chrono::system_clock::time_point& Room::creation_date() const {
    return this->_created_at;
}

size_t Room::player_limit() const {
    return this->_player_limit;
}

void Room::add_spectator(const std::shared_ptr<User>& spectator, bool waitlist = true) {
    if (this->find_player(spectator->id())) {
        throw GameException(GameException::SPECTATOR_IS_PLAYER);
    }

    // Add the spectator if they aren't already in the room.
    if (std::find(this->_spectators.begin(), this->_spectators.end(), spectator) == this->_spectators.end()) {
        this->_spectators.emplace_back(spectator);
        spectator->_status = waitlist ? User::Status::SPECTATOR_WAITLIST : User::Status::SPECTATOR;
    }
}

void Room::remove_spectator(const User& spectator) {
    this->remove_spectator(spectator.id());
}

void Room::remove_spectator(User::Id spectator) {
    // Find the spectator in the room.
    auto find = std::find_if(this->_spectators.begin(), this->_spectators.end(), [spectator](const std::shared_ptr<User>& spec) {
        return spec->id() == spectator;
    });

    if (find == this->_spectators.end()) {
        throw GameException(GameException::USER_NOT_FOUND);
    }

    // Remove the spectator.
    (*find)->_status = User::Status::UNKNOWN;
    this->_spectators.erase(find);
}

void Room::add_player(const std::shared_ptr<User>& player) {
    if (this->find_spectator(player->id())) {
        throw GameException(GameException::PLAYER_IS_SPECTATOR);
    }

    if (this->is_full()) {
        throw GameException(GameException::ROOM_FULL);
    }

    // Silently succeed if the player is already in the room.
    if (std::find(this->_players.begin(), this->_players.end(), player) != this->_players.end()) {
        return;
    }

    // Add the owner if nobody is an owner.
    if (!this->_owner) {
        this->_owner = player;
    }

    // Add the player.
    this->_players.push_back(player);
    player->_status = User::Status::UNKNOWN;

    // Broadcast and send an event to the game instance.
    this->broadcast_message(std::string("[+] ") + std::string(player->name()) + std::string(" joined the room."));
    // TODO(ethan): Player join event.
}

void Room::remove_player(const User& player) {
    this->remove_player(player.id());
}

void Room::remove_player(User::Id player) {
    // Find the player in the room.
    auto find = std::find_if(this->_players.begin(), this->_players.end(), [player](const std::shared_ptr<User>& play) {
        return play->id() == player;
    });

    if (find == this->_players.end()) {
        throw GameException(GameException::USER_NOT_FOUND);
    }

    // Change the owner if the player was the owner.
    if (this->_owner && (player == (*this->_owner)->id())) {
        auto next = std::find_if(this->_players.begin(), this->_players.end(), [&player](const std::shared_ptr<User>& play) {
            return play->id() != player;
        });

        if (next == this->_players.end()) {
            this->_owner = std::nullopt;
        } else {
            this->set_owner(*next);
        }
    }

    // Remove the player if they are in the room.
    (*find)->_status = User::Status::UNKNOWN;
    this->_players.erase(find);

    // Broadcast and send an event to the game instance.
    this->broadcast_message(std::string("[-] ") + std::string((*find)->name()) + std::string(" left the room."));
    // TODO(ethan): Player leave event.

    // Replace player slot with spectator.
    if (!this->is_full()) {
        this->_join_spectator();
    }
}

void Room::set_player_limit(size_t limit, LimitBehavior behavior) {
    for (size_t i = this->_players.size(); i > limit; i--) {
        auto player = this->_players[limit];
        this->remove_player(*player);
        if (behavior == LimitBehavior::SPECTATE) {
            this->add_spectator(player);
        }
    }
}

void Room::set_owner(std::shared_ptr<User> owner) {
    // TODO(ethan): Owner change event.

    this->_owner = owner;
}

void Room::update_game() {
    // TODO(ethan): Update game call.
}

std::optional<std::shared_ptr<User>> Room::find_player(User::Name player) const {
    return arepa::find_if(this->_players, [&player](const std::shared_ptr<User>& other) {
        return player == other->name();
    });
}

std::optional<std::shared_ptr<User>> Room::find_player(User::Id player) const {
    return arepa::find_if(this->_players, [&player](const std::shared_ptr<User>& other) {
        return player == other->id();
    });
}

std::optional<std::shared_ptr<User>> Room::find_spectator(User::Name spectator) const {
    return arepa::find_if(this->_spectators, [&spectator](const std::shared_ptr<User>& other) {
        return spectator == other->name();
    });
}

std::optional<std::shared_ptr<User>> Room::find_spectator(User::Id spectator) const {
    return arepa::find_if(this->_spectators, [&spectator](const std::shared_ptr<User>& other) {
        return spectator == other->id();
    });
}

void Room::remove_player_or_spectator(std::shared_ptr<User> player_or_spectator) {
    if (player_or_spectator->is_spectator()) {
        this->remove_spectator(*player_or_spectator);
    } else {
        this->remove_player(*player_or_spectator);
    }
}

void Room::broadcast_message(const std::string& message) {
    for (auto& user : this->_players) {
        user->send(message);
    }

    for (auto& user : this->_spectators) {
        user->send(message);
    }
}

bool Room::process_command(User::Id player, const arepa::command::Command& command) {
    // Get the player object.
    std::optional<std::shared_ptr<User>> playerPtr = this->find_player_or_spectator(player);
    if (!playerPtr) {
        // TODO(ethan): Log the warning.
        return false;
    }

    // Try processing it through the game instance.
    if ((**playerPtr).is_playing()) {
        // Player isn't in the room.
        // TODO(ethan): Try game instance command.
    }

    // Get the executor for the command.
    auto executor = this->commands.find(command.name());
    if (!executor) {
        return false;
    }

    // Execute the command.
    (*executor)->execute(*this, **playerPtr, command.arguments());
    return true;
}

bool Room::process_message(User::Id player, const std::string& message) {
    // Get the player object.
    std::optional<std::shared_ptr<User>> find = this->find_player_or_spectator(player);
    if (!find) {
        // Player isn't in the room.
        // TODO(ethan): Log the warning.
        return false;
    }

    // Try processing it through the game instance.
    auto playerPtr = *find;
    if (playerPtr->is_playing()) {
        // Player isn't in the room.
        // TODO(ethan): Try game instance messages.
    }

    // Create a regular player message.
    std::string messagePrefix = playerPtr->is_spectator() ? "[spectator] " : "";
    std::string messageSender = playerPtr->name();
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
