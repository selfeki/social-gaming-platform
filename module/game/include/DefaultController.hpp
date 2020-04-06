#pragma once

#include "Controller.hpp"

namespace arepa::game {

/**
 * Some default implementations for the game controller.
 */
class DefaultController : public Controller {

#pragma mark - Fields -
protected:
    Environment* room;

#pragma mark - Methods (Controller) -
public:
    void initialize(Environment& environment) override;
    Intercept intercept_spectator_message(Spectator& spectator, const std::string& message) override;
    Intercept intercept_spectator_command(Spectator& spectator, const Command& command) override;
    void on_player_quit(Player& player) override;
    arepa::Result<void, std::string> on_option_change(const std::string& option, const std::string& value) override;
};

}
