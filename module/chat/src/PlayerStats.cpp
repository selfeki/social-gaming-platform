#include "PlayerStats.hpp"

using namespace arepa::chat;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

PlayerStats::PlayerStats()
    : _wins(0)
    , _games(0) {}

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Methods -
// ---------------------------------------------------------------------------------------------------------------------

unsigned int PlayerStats::wins() const {
    return this->_wins;
}

unsigned int PlayerStats::games() const {
    return this->_games;
}

void PlayerStats::increment_wins() {
    this->_wins++;
}

void PlayerStats::increment_games() {
    this->_games++;
}
