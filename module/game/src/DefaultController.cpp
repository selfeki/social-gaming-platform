#include "DefaultController.hpp"

using namespace arepa::game;
using arepa::chat::Player;
using arepa::chat::Spectator;
using arepa::command::Command;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

void DefaultController::initialize(Environment& environment) {
    this->room = &environment;
}

Controller::Intercept DefaultController::intercept_spectator_message(Spectator& spectator, const std::string& message) {
    return Intercept::DEFAULT;
}

Controller::Intercept DefaultController::intercept_spectator_command(Spectator& spectator, const Command& command) {
    return Intercept::DEFAULT;
}

void DefaultController::on_player_quit(Player& player) {
    this->room->end_game();
}

arepa::Result<void, std::string> DefaultController::on_option_change(const std::string& option, const std::string& value) {
    return arepa::Error<std::string>("This game does not take any options.");
}