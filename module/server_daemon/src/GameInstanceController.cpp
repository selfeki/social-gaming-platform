#include "GameInstanceController.hpp"

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
    this->_game_instance->updateState();

    std::vector<gameSpecification::ControllerOutgoingMessage> 
    player_messages = this->_game_instance->popOutgoingPlayerMessages();

    std::vector<gameSpecification::ControllerInputRequest>
    requests = this->_game_instance->getInputRequests(); 

    const std::set<arepa::chat::MemberPtr<Player>>& players = this->room->players();

    for(auto& message : player_messages) {

        //TODO: use Ethan's room->find_player() command. We need the ID for that though, so we may need to
        //associate name and id in the game instance
        auto recipient = std::find_if(players.begin(), players.end(), 
                        [&message] (auto& p) {
                            return *(p.user()->name()) == message.to;
                        });


        if(recipient != players.end()) {
            (*recipient).user()->send_message(message.message);
        }
    }

    for(auto& request : requests) {

        //TODO: use Ethan's room->find_player() command. We need the ID for that though, so we may need to
        //associate name and id in the game instance
        auto recipient = std::find_if(players.begin(), players.end(), 
                        [&request] (auto& p) {
                            return *(p.user()->name()) == request.to;
                        });

        //this is probably an inefficient way to do this... should probably
        //only store one message for each player recieving the same message instead of making a new one everytime
        std::string constructed_message = request.prompt + "\n\n";
        for(auto& choice : request.choices) {
            constructed_message += (choice + "\n");
        }
        constructed_message += "\n";

        if(recipient != players.end()) {
            (*recipient).user()->send_message(constructed_message);
        }

    }

}

void GameInstanceController::start(){
    this->_game_instance->loadPlayers(this->room->players());
    this->_game_instance->loadAudience(this->room->spectators());
}

void GameInstanceController::loadInstance(std::unique_ptr<gameSpecification::GameInstance> _game_instance) {



}
