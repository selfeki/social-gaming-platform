#include "Room.hpp"

#include "GameException.hpp"

using namespace arepa::game::room;

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
    auto next = std::find_if(this->_spectators.begin(), this->_spectators.end(), [](const Spectator& spec) {
        return spec.can_join;
    });

    if (next != this->_spectators.end()) {
        this->add_player(next->player);
        this->_spectators.erase(next);
    }
}

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

const Room::Id& Room::id() const {
    return this->_id;
}

std::optional<std::shared_ptr<Player>> Room::owner() const {
    return this->_owner;
}

bool Room::is_full() const {
    return this->_players.size() >= this->player_limit();
}

const std::vector<std::shared_ptr<Player>>& Room::players() const {
    return this->_players;
}

const std::vector<std::shared_ptr<Player>>& Room::players_in_game() const {
    // TODO(ethan): When playing a game.
    return this->players();
}

const std::vector<Spectator>& Room::spectators() const {
    return this->_spectators;
}

const std::chrono::system_clock::time_point& Room::creation_date() const {
    return this->_created_at;
}

size_t Room::player_limit() const {
    return this->_player_limit;
}

void Room::add_spectator(Spectator spectator) {
    // Add the spectator if they aren't already in the room.
    if (std::find(this->_spectators.begin(), this->_spectators.end(), spectator) == this->_spectators.end()) {
        this->_spectators.emplace_back(std::move(spectator));
    }
}

void Room::remove_spectator(const Spectator& spectator) {
    this->remove_spectator(spectator.player->id());
}

void Room::remove_spectator(Spectator::Id spectator) {
    // Remove the spectator if they are in the room.
    auto find = std::find(this->_spectators.begin(), this->_spectators.end(), spectator);
    if (find != this->_spectators.end()) {
        throw GameException(GameException::PLAYER_NOT_FOUND);
    }

    this->_spectators.erase(find);
}

void Room::add_player(std::shared_ptr<Player> player) {
    if (this->is_full()) {
        throw GameException(GameException::ROOM_FULL);
    }

    // Add the player if they aren't already in the room.
    if (std::find(this->_players.begin(), this->_players.end(), player) == this->_players.end()) {
        this->_spectators.emplace_back(std::move(player));
    }

    // TODO(ethan): Player join event.
}

void Room::remove_player(const std::shared_ptr<Player>& player) {
    this->remove_player(player->id());
}

void Room::remove_player(Player::Id player) {
    auto find = std::find(this->_players.begin(), this->_players.end(), player);
    if (find != this->_players.end()) {
        throw GameException(GameException::PLAYER_NOT_FOUND);
    }

    // Change the owner if the player was the owner.
    if (this->_owner && (player == (*this->_owner)->id())) {
        auto next = std::find_if(this->_players.begin(), this->_players.end(), [&player](const std::shared_ptr<Player>& play) {
            return play->id() != player;
        });

        if (next == this->_players.end()) {
            this->_owner = std::nullopt;
        } else {
            this->set_owner(*next);
        }
    }

    // Remove the player if they are in the room.
    this->_players.erase(find);
    // TODO(ethan): Player leave event.

    // Replace player slot with spectator.
    if (!this->is_full()) {
        this->_join_spectator();
    }
}

void Room::set_player_limit(size_t limit, LimitBehavior behavior) {
    for (size_t i = this->_players.size(); i > limit; i--) {
        auto player = this->_players[limit];
        this->remove_player(player);
        if (behavior == LimitBehavior::SPECTATE) {
            this->add_spectator(Spectator(player));
        }
    }
}

void Room::set_owner(std::shared_ptr<Player> owner) {
    // TODO(ethan): Owner change event.

    this->_owner = owner;
}

void Room::update_game() {
    // TODO(ethan): Update game call.
}

std::optional<std::shared_ptr<Player>> Room::find_player(Player::Name player) {
    auto find = std::find_if(this->_players.begin(), this->_players.end(), [&player](const std::shared_ptr<Player>& other) {
        return player == other->name();
    });

    if (find == this->_players.end()) {
        return std::nullopt;
    } else {
        return { *find };
    }
}

std::optional<std::shared_ptr<Player>> Room::find_player(Player::Id player) {
    auto find = std::find_if(this->_players.begin(), this->_players.end(), [&player](const std::shared_ptr<Player>& other) {
        return player == other->id();
    });

    if (find == this->_players.end()) {
        return std::nullopt;
    } else {
        return { *find };
    }
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
