#pragma once

#include <arepa/game/DefaultController.hpp>
#include "GameInstance.h"


/**
 * Quick and dirty game instance controller (interface between game instance and room)
 */

class GameInstanceController : public arepa::game::DefaultController { 

protected:

    std::set<arepa::chat::Player::Id> _players;
    std::set<arepa::chat::Spectator::Id> _audience;
    std::unique_ptr<gameSpecification::GameInstance> _game_instance;
    

public:

    virtual ~GameInstanceController() = default;

public:

    void initialize(arepa::game::Environment& environment) override;
    Intercept intercept_player_message(Player& player, const std::string& message) override;
    Intercept intercept_player_command(Player& player, const Command& command) override;
    void on_player_quit(Player& player) override;
    arepa::Result<void, std::string> on_option_change(const OptionKey& option, const OptionValue& value) override;
    std::vector<std::pair<OptionKey, std::string>> list_option_descriptions() override;
    void update() override;
    void start() override;

    /*
    *Load a game instance into this controller
    * 
    * */
    void registerInstance(std::unique_ptr<gameSpecification::GameInstance> _game_instance);

};
