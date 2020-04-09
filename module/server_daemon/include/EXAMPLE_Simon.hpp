#pragma once

#include <arepa/game/DefaultController.hpp>

#include <set>

/**
 * An example Simon-says game.
 *
 * This differs from normal simon says in that you simply have to perfectly re-type what Simon says.
 * The difficulty comes from when you make typos ;)
 */
class EXAMPLE_Simon : public arepa::game::DefaultController {

#pragma mark - Fields -
protected:
    arepa::chat::Player::Id _simon;
    std::string _simon_message;
    std::set<arepa::chat::Player::Id> _incomplete;


#pragma mark - Constructors -
public:
    virtual ~EXAMPLE_Simon() = default;


#pragma mark - Methods (Protected) -
protected:
    /**
     * Ends the current round.
     * This will disqualify all the players who didn't finish.
     */
    void end_round();

    /**
     * Prepares the next round.
     * @param message The message.
     */
    void next_round(std::string message);

    /**
     * Check if a player wins.
     */
    void check_win();

    /**
     * Gets the current Simon's name.
     * @return The Simon's name.
     */
    std::string simon_name();


#pragma mark - Methods (Controller) -
public:
    void initialize(arepa::game::Environment& environment) override;
    Intercept intercept_player_message(Player& player, const std::string& message) override;
    Intercept intercept_player_command(Player& player, const Command& command) override;
    void on_player_quit(Player& player) override;
    arepa::Result<void, std::string> on_option_change(const OptionKey& option, const OptionValue& value) override;
    std::vector<std::pair<OptionKey, std::string>> list_option_descriptions() override;
    void update() override;
    void start() override;

};
