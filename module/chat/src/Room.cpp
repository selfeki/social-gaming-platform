#include "Room.hpp"

#include "ChatException.hpp"

#include <arepa/Util.hpp>

#include <algorithm>
#include <queue>

using Game = arepa::game::Controller;

using namespace arepa::chat;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - UserContainerGuard -
// ---------------------------------------------------------------------------------------------------------------------

Room::MemberGuard::MemberGuard(arepa::chat::Room* room, Member& member)
    : _room(*room)
    , _member(member) {
    this->_room._cached_remove(this->_member);
}

Room::MemberGuard::~MemberGuard() {
    this->_room._cached_add(this->_member);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

Room::Room(Room::Id id)
    : _owner(nullptr)
    , _created_at(std::chrono::system_clock::now())
    , _id(std::move(id))
    , _player_limit(8) {
}

Room::Room()
    : Room(Room::Id::generate(5)) {}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Private) -
// ---------------------------------------------------------------------------------------------------------------------

const Member* Room::_find_member(const Room::User::Id& id) const {
    auto find = this->_members.find(id);
    if (find == this->_members.end()) {
        return nullptr;
    }

    return &(find->second);
}

const Member* Room::_find_member(const Room::User::Name& name) const {
    for (const auto& entry : this->_members) {
        if (entry.second->name() == name) {
            return &entry.second;
        }
    }

    return nullptr;
}

const Member* Room::_find_member(const std::shared_ptr<User>& user) const {
    return this->_find_member(user->id());
}

void Room::_cached_remove(const Member& member) {
    this->_cached_users.erase(MemberPtr<User>(const_cast<Member*>(&member)));

    if (member.is_disqualified()) {
        this->_players_disqualified--;
    }

    if (member.is_spectator() || member.is_disqualified()) {
        this->_cached_spectators.erase(MemberPtr<Spectator>(const_cast<Member*>(&member)));
    }

    if (member.is_player() && !member.is_disqualified()) {
        this->_cached_players.erase(MemberPtr<Player>(const_cast<Member*>(&member)));
    }
}

void Room::_cached_add(Member& member) {
    this->_cached_users.insert(MemberPtr<User>(&member));

    if (member.is_disqualified()) {
        this->_players_disqualified++;
    }

    if (member.is_spectator() || member.is_disqualified()) {
        this->_cached_spectators.insert(MemberPtr<Spectator>(&member));
    }

    if (member.is_player() && !member.is_disqualified()) {
        this->_cached_players.insert(MemberPtr<Player>(&member));
    }
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

const Room::Id& Room::id() const {
    return this->_id;
}

MemberPtr<User> Room::owner() const {
    return this->_owner;
}

bool Room::is_full() const {
    return this->player_count() >= this->player_limit();
}

const std::set<MemberPtr<Room::User>>& Room::users() const {
    return this->_cached_users;
}

const std::set<MemberPtr<Player>>& Room::players() const {
    return this->_cached_players;
}

const std::set<MemberPtr<Spectator>>& Room::spectators() const {
    if (this->is_game_active()) {
        return this->_cached_spectators;
    }

    // Warning: Super dangerous territory.
    //
    // The template parameter of MemberPtr only changes the return type of a few functions within it.
    // Consequently, every form of it has a compatible vtable and data member offset.
    //
    // We're going to use some extremely dirty casting to skip having to needlessly copy things.
    // This is perfectly safe as long as nobody changes the implementation of MemberPtr.
    return *reinterpret_cast<const std::set<MemberPtr<Spectator>>*>(reinterpret_cast<const void*>(&this->_cached_users));
}

const std::chrono::system_clock::time_point& Room::creation_date() const {
    return this->_created_at;
}

size_t Room::player_limit() const {
    return this->_player_limit;
}

size_t Room::player_count() const {
    return this->_cached_players.size() + this->_players_disqualified;
}

size_t Room::active_player_count() const {
    return this->_cached_players.size();
}

size_t Room::spectator_limit() const {
    return this->_spectator_limit;
}

size_t Room::spectator_count() const {
    return this->_cached_spectators.size() - this->_players_disqualified;
}

size_t Room::user_count() const {
    return this->_members.size();
}

void Room::_remove_user(const std::shared_ptr<User>& user) {
    auto member = this->_find_member(user);
    if (!member) {
        // The user isn't in the room.
        // TODO(ethan): Log a warning.
        return;
    }

    // Fire signal for leave event.
    this->on_member_leave(member);

    // Remove the user from the cached lists.
    this->_cached_remove(*member);

    // Remove the user from the users map.
    this->_members.erase(user->id());

    // Notify the game instance (if they were a player).
    if (this->is_game_active() && (member->is_player() || member->is_disqualified())) {
        this->_game->on_player_quit(*member->user());
    }

    // Flush the waitlist.
    this->flush_waitlist();

    // Shift the owner to the next person.
    auto owner = this->owner();
    if (owner && owner == user) {
        if (this->_members.empty()) {
            this->_owner = nullptr;
        } else {
            this->set_owner(*this->_cached_users.begin());
        }
    }
}

void Room::add_user(const std::shared_ptr<User>& user, Room::Join method) {
    size_t current_players = this->player_count() - (this->find_player(user->id()) ? 1 : 0);
    size_t current_spectators = this->spectator_count() - (this->find_spectator(user->id()) ? 1 : 0);

    Member::Status status;
    switch (method) {
    case Room::Join::AS_PLAYER:

        if (current_players >= this->player_limit()) {
            throw ChatException(ChatException::ROOM_FULL);
        }

        if (this->is_game_active()) {
            throw ChatException(ChatException::NOT_ALLOWED_DURING_GAME);
        }

        status = Member::Status::PLAYER;
        break;

    case Room::Join::AS_SPECTATOR:
        if (current_spectators < this->spectator_limit()) {
            status = Member::Status::SPECTATOR;
            break;
        }

        throw ChatException(ChatException::ROOM_FULL);

    case Room::Join::AS_PLAYER_WAITING:
        if (current_players < this->player_limit()) {
            status = Member::Status::PLAYER;
            break;
        } else if (current_spectators < this->spectator_limit()) {
            status = Member::Status::SPECTATOR;
            break;
        }

        throw ChatException(ChatException::ROOM_FULL);
    }

    // Update the existing user container, or add a new user container.
    Member* member = this->_find_member(user);
    if (member) {
        MemberGuard guard(this, *member);
        member->status = status;
    } else {
        this->_members.insert({ user->id(), Member(user, status) });
        member = this->_find_member(user);
        member->suppress_broadcasts = true;
        this->_cached_add(*member);

        // If there isn't an owner, make the new user an owner.
        if (!this->_owner) {
            this->set_owner(MemberPtr<User>(member));
        }

        // Fire signal for join event.
        this->on_member_join(member);
        member->suppress_broadcasts = false;
    }
}

void Room::flush_waitlist() {
    if (this->is_game_active() || this->spectator_count() == 0) {
        return;
    }

    // Collect a list of waitlisted users.
    std::queue<Member*> waitlisted;
    for (auto& spectator : this->spectators()) {
        auto member = static_cast<Member*>(spectator);
        if (member->is_waitlisted()) {
            waitlisted.push(member);
        }
    }

    // Flush the users.
    while (!waitlisted.empty() && this->player_count() < this->player_limit()) {
        Member* member = waitlisted.front();
        waitlisted.pop();

        MemberGuard guard(this, *member);
        member->status = Member::Status::PLAYER;
    }
}

bool Room::is_game_active() const {
    return this->_game_started;
}

bool Room::is_game_loaded() const {
    return this->_game != nullptr;
}

void Room::update_game() {
    if (this->is_game_active()) {
        this->_game->update();
    }
}

void Room::set_player_limit(size_t limit) {
    if (limit < 1) {
        throw ChatException(ChatException::NOT_ALLOWED_OPTION, "Player limit must be positive.");
    }

    ssize_t overflow = this->player_count() - limit;
    if (overflow > 0) {
        std::vector<MemberPtr<Player>> remove(this->_cached_players.begin(), std::next(this->_cached_players.begin(), overflow));

        // Perform the removal.
        for (auto& member : remove) {
            MemberGuard guard(this, *static_cast<Member*>(member));
            static_cast<Member*>(member)->status = Member::Status::PLAYER_WAITLIST;
        }
    }
}

void Room::set_spectator_limit(size_t limit) {
    ssize_t overflow = this->spectator_count() - limit;

    if (overflow > 0) {
        std::vector<Member*> remove;
        for (auto& pair : this->_members) {
            if (pair.second.is_player()) {
                remove.push_back(&pair.second);
            }
        }

        // Sort the spectators.
        // Order:
        // - Not waitlisted.
        // - Most recently joined.
        std::sort(remove.begin(), remove.end(), [](const auto& a, const auto& b) {
            if (a->is_waitlisted() && !b->is_waitlisted()) {
                return true;
            }

            return *a < *b;
        });

        // Trim the vector.
        remove.erase(remove.begin() + overflow, remove.end());

        // Perform the removal.
        for (const auto& user : remove) {
            this->_remove_user(user->user());
        }
    }
}

MemberPtr<Player> Room::find_player(const std::shared_ptr<User>& player) const {
    return this->find_player(player->id());
}

MemberPtr<Player> Room::find_player(Room::User::Id player) const {
    auto find = std::find_if(this->_cached_players.begin(), this->_cached_players.end(), [&player](const auto& other) {
        return other->id() == player;
    });

    return find == this->_cached_players.end() ? nullptr : *find;
}

MemberPtr<Player> Room::find_player(Room::User::Name player) const {
    auto find = std::find_if(this->_cached_players.begin(), this->_cached_players.end(), [&player](const auto& other) {
        return other->name() == player;
    });

    return find == this->_cached_players.end() ? nullptr : *find;
}

MemberPtr<Spectator> Room::find_spectator(const std::shared_ptr<User>& spectator) const {
    return this->find_spectator(spectator->id());
}

MemberPtr<Spectator> Room::find_spectator(User::Id spectator) const {
    auto find = std::find_if(this->spectators().begin(), this->spectators().end(), [&spectator](const auto& other) {
        return other->id() == spectator;
    });

    return find == this->spectators().end() ? nullptr : *find;
}

MemberPtr<Spectator> Room::find_spectator(User::Name spectator) const {
    auto find = std::find_if(this->spectators().begin(), this->spectators().end(), [&spectator](const auto& other) {
        return other->name() == spectator;
    });

    return find == this->spectators().end() ? nullptr : *find;
}

MemberPtr<User> Room::find_user(const std::shared_ptr<User>& user) const {
    return this->find_user(user->id());
}

MemberPtr<User> Room::find_user(User::Id user) const {
    auto find = std::find_if(this->_cached_users.begin(), this->_cached_users.end(), [&user](const auto& other) {
        return other->id() == user;
    });

    return find == this->_cached_users.end() ? nullptr : *find;
}

MemberPtr<User> Room::find_user(User::Name user) const {
    auto find = std::find_if(this->_cached_users.begin(), this->_cached_users.end(), [&user](const auto& other) {
        return other->name() == user;
    });

    return find == this->_cached_users.end() ? nullptr : *find;
}

void Room::broadcast_message(const std::string& message) {
    for (auto& user : this->_cached_users) {
        if (!static_cast<Member*>(user)->suppress_broadcasts) {
            user->send_message(message);
        }
    }
}

void Room::broadcast_message_to_spectators(const std::string& message) {
    for (auto& user : this->spectators()) {
        if (!static_cast<Member*>(user)->suppress_broadcasts) {
            user->send_message(message);
        }
    }
}

void Room::broadcast_packet(const User::Packet& packet) {
    for (auto& user : this->_cached_users) {
        if (!static_cast<Member*>(user)->suppress_broadcasts) {
            user->send_packet(packet);
        }
    }
}

void Room::broadcast_packet_to_spectators(const User::Packet& packet) {
    for (auto& user : this->spectators()) {
        if (!static_cast<Member*>(user)->suppress_broadcasts) {
            user->send_packet(packet);
        }
    }
}

bool Room::process_command(Room::User::Id user, const arepa::command::Command& command) {
    auto member = this->_find_member(user);
    if (!member) {
        // TODO(ethan): Log the warning.
        return false;
    }

    // Get the executor for the command (if it's a protected command).
    {
        auto executor = this->commands.find(command.name());
        if (executor) {
            (*executor)->execute(*this, *member->user(), command.arguments());
            return true;
        }
    }

    // Try processing it through the game instance.
    if (this->is_game_active()) {
        Game::Intercept result = (member->is_player() && !member->is_disqualified())
            ? this->_game->intercept_player_command(*member->user(), command)
            : this->_game->intercept_spectator_command(*member->user(), command);

        if (result == Game::Intercept::INTERCEPTED) {
            return true;
        }
    }

    // Get the executor for the command.
    auto executor = this->commands.find(command.name());
    if (executor) {
        (*executor)->execute(*this, *member->user(), command.arguments());
        return true;
    }

    return false;
}

bool Room::process_message(Room::User::Id user, const std::string& message) {
    auto member = this->_find_member(user);
    if (!member) {
        // TODO(ethan): Log the warning.
        return false;
    }

    // Try processing it through the game instance.
    if (this->is_game_active() && member->is_player()) {
        Game::Intercept result = (member->is_player() && !member->is_disqualified())
            ? this->_game->intercept_player_message(*member->user(), message)
            : this->_game->intercept_spectator_message(*member->user(), message);

        if (result == Game::Intercept::INTERCEPTED) {
            return true;
        }
    }

    // Create a regular player message.
    std::string messagePrefix = (this->is_game_loaded() && (member->is_spectator() || member->is_disqualified())) ? "[spectator] " : "";
    std::string messageSender = (*member)->name();
    std::string outbound = messagePrefix + messageSender + ": " + message;

    // Handle it normally.
    this->broadcast_message(outbound);
    return true;
}

void Room::load_game(std::unique_ptr<arepa::game::Controller> game) {
    if (this->_game != nullptr && this->_game_started) {
        this->end_game();
    }

    this->_game = std::move(game);
    this->_game->initialize(*this);
}

void Room::start_game() {
    if (!this->is_game_loaded()) {
        throw ChatException(ChatException::REQUIRES_GAME);
    }

    this->reset_players();

    for (auto& player : this->players()) {
        player->stats().increment_wins();
    }

    this->_game_started = true;
    this->_game->start();
}

void Room::end_game() {
    this->reset_players();
    this->_game_started = false;
}

arepa::Result<void, std::string> Room::game_option(const arepa::game::Controller::OptionKey& option, const arepa::game::Controller::OptionValue& value) {
    if (!this->is_game_loaded()) {
        throw ChatException(ChatException::REQUIRES_GAME);
    }

    return this->_game->on_option_change(option, value);
}


arepa::game::Controller& Room::game() const {
    if (this->_game == nullptr) {
        throw ChatException(ChatException::REQUIRES_GAME);
    }

    return *this->_game;
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Environment) -
// ---------------------------------------------------------------------------------------------------------------------

void Room::disqualify_player(const arepa::chat::MemberPtr<arepa::chat::Player>& player) {
    if (!player.is_player()) {
        // FIXME(ethan): Maybe this should throw intead?
        return;
    }

    auto member = static_cast<Member*>(player);
    MemberGuard guard(this, *member);
    member->disqualified = true;
}

void Room::reset_players() {
    for (auto& pair : this->_members) {
        Member& member = pair.second;
        if (member.disqualified) {
            MemberGuard guard(this, member);
            member.disqualified = false;
        }
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
