#include "EXAMPLE_Simon.hpp"

#include <arepa/game/GameException.hpp>

#include <algorithm>
#include <boost/algorithm/string/trim.hpp>

using arepa::Result;
using arepa::chat::Player;
using arepa::chat::Spectator;
using arepa::command::Command;
using arepa::game::Environment;
using arepa::game::GameException;
using boost::algorithm::trim_copy;
using Intercept = arepa::game::Controller::Intercept;
using OptionKey = arepa::game::Controller::OptionKey;
using OptionValue = arepa::game::Controller::OptionValue;


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

std::string EXAMPLE_Simon::simon_name() {
    return std::string(this->room->find_player(this->_simon)->name());
}

void EXAMPLE_Simon::check_win() {
    if (this->room->active_player_count() == 1) {
        this->room->broadcast_message("[SIMON]: Game over! Nobody could keep up with " + this->simon_name());
        this->room->end_game();
    } else if (this->room->active_player_count() == 2) {
        auto winner = *std::find_if(this->room->players().begin(), this->room->players().end(), [this](const auto& player) {
            return player->id() != this->_simon;
        });

        this->room->broadcast_message("[SIMON]: Game over! " + std::string(winner->name()) + " wins!");
        winner->stats().increment_wins();
    }
}

void EXAMPLE_Simon::end_round() {
    // Everybody who didn't complete the round will be disqualified.
    for (const auto& fail : this->_incomplete) {
        auto player = this->room->find_player(fail);
        this->room->broadcast_message("[SIMON]: " + std::string(player->name()) + " is disqualified! They couldn't keep up.");
        this->room->disqualify_player(this->room->find_player(fail));
    }

    this->check_win();
}

void EXAMPLE_Simon::next_round(std::string message) {
    this->_simon_message = std::move(message);
    this->_incomplete.clear();
    for (auto& player : this->room->players()) {
        if (player->id() != this->_simon) {
            this->_incomplete.insert(player->id());
        }
    }

    this->room->broadcast_message("[SIMON]: " + this->simon_name() + " says: " + this->_simon_message);
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods (Controller) -
// ---------------------------------------------------------------------------------------------------------------------

void EXAMPLE_Simon::initialize(Environment& environment) {
    arepa::game::DefaultController::initialize(environment);

    this->_simon = environment.owner()->id();

    this->room->broadcast_message("----------------------------------------------------------------------------");
    this->room->broadcast_message("Welcome to The Game of Simon!");
    this->room->broadcast_message("The rules are simple:");
    this->room->broadcast_message("");
    this->room->broadcast_message("One person is simon, and he or she says things.");
    this->room->broadcast_message("The rest of you need to repeat what was said by Simon without making a typo.");
    this->room->broadcast_message("");
    this->room->broadcast_message("Good luck!");
    this->room->broadcast_message("----------------------------------------------------------------------------");
}

Intercept EXAMPLE_Simon::intercept_player_message(Player& player, const std::string& message) {
    // Simon said.
    // Start the next round.
    if (player.id() == this->_simon) {
        this->end_round();
        if (this->room->is_game_active()) {
            this->next_round(trim_copy(message));
        }
        return Intercept::INTERCEPTED;
    }

    // A player answered.
    if (this->_incomplete.find(player.id()) != this->_incomplete.end()) {
        if (trim_copy(message) != this->_simon_message) {
            this->room->broadcast_message("[SIMON]: " + std::string(player.name()) + " is disqualified! They said: " + message);
            this->room->disqualify_player(this->room->find_player(player.id()));
            this->check_win();
        }

        this->_incomplete.erase(player.id());
        return Intercept::INTERCEPTED;
    }

    // A player said... nothing.
    player.send_message("If you want to speak in the Simon game, use /say");
    return Intercept::INTERCEPTED;
}

Intercept EXAMPLE_Simon::intercept_player_command(Player& player, const Command& command) {
    // Introduce a /say command to allow players to still speak.
    if (command.name() == "say" || command.name() == "s") {
        if (!command.raw_arguments().empty()) {
            this->room->broadcast_message(std::string(player.name()) + ": " + command.raw_arguments());
        }
        return Intercept::INTERCEPTED;
    }

    return Intercept::DEFAULT;
}

void EXAMPLE_Simon::on_player_quit(Player& player) {
    // End the game if Simon leaves.
    if (player.id() == this->_simon) {
        this->room->broadcast_message("Simon left during the game!");
        this->room->broadcast_message("The game is now cancelled.");
        this->room->end_game();
    }
}

Result<void, std::string> EXAMPLE_Simon::on_option_change(const OptionKey& option, const OptionValue& value) {
    if (option == "simon") {
        auto name = arepa::chat::User::Name::parse(value);
        if (!name) {
            return arepa::Error("Unable to set '" + value + "' as simon.");
        }

        auto find = this->room->find_player(*name);
        if (!find) {
            if (this->room->find_spectator(*name)) {
                return arepa::Error("Unable to set '" + value + "' as simon. They are a spectator.");
            } else {
                return arepa::Error("Unable to set '" + value + "' as simon. They aren't in the room.");
            }
        }

        this->_simon = find->id();
        this->room->owner()->send_system_message(value + " is the new simon.");
        return arepa::Ok();
    }

    return arepa::Ok();
}

std::vector<std::pair<OptionKey, std::string>> EXAMPLE_Simon::list_option_descriptions() {
    return { { "simon", "The player who is acting as Simon." } };
}

void EXAMPLE_Simon::update() {
    // Intentionally do nothing, since the game flow is based on Simon.
}

void EXAMPLE_Simon::start() {
    // Assert playing conditions.
    if (this->room->player_count() < 3) {
        throw GameException(GameException::OTHER, "This game requires at least three players.");
    }

    if (!this->room->find_player(this->_simon)) {
        this->_simon = this->room->owner()->id();
        throw GameException(GameException::OTHER, "The previous simon left. Either set a simon, or try again to use the room owner.");
    }

    // Reset game data.
    this->_incomplete.clear();
    this->_simon_message = "";

    // Start game.
    this->room->broadcast_message("[SIMON]: Simon is " + this->simon_name());
    this->room->broadcast_message("[SIMON]: Good luck!");
}
