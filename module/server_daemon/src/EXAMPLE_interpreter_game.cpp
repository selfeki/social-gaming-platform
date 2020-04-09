#include "EXAMPLE_interpreter_game.hpp"

using arepa::Result;
using arepa::chat::Player;
using arepa::chat::Spectator;
using arepa::command::Command;
using arepa::game::Environment;
using Intercept = arepa::game::Controller::Intercept;
using OptionKey = arepa::game::Controller::OptionKey;
using OptionValue = arepa::game::Controller::OptionValue;




void GameInstanceController::initialize(Environment& environment) {
    arepa::game::DefaultController::initialize(environment);

    this->_game_instance->loadPlayers(this->room->players());
    this->_game_instance->loadAudience(this->room->spectators());

}

Intercept GameInstanceController::intercept_player_message(Player& player, const std::string& message){
    
    


}

Intercept GameInstanceController::intercept_player_command(Player& player, const Command& command){
    
}

void GameInstanceController::on_player_quit(Player& player){

}

arepa::Result<void, std::string> GameInstanceController::on_option_change(const OptionKey& option, const OptionValue& value){

}

std::vector<std::pair<OptionKey, std::string>> GameInstanceController::list_option_descriptions(){
    
}

void GameInstanceController::update(){


}

void GameInstanceController::start(){
    

}

void GameInstanceController::registerInstance(std::unique_ptr<gameSpecification::GameInstance> _game_instance) {



}
