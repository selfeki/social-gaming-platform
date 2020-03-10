#include "PlayerStats.hpp"

using namespace arepa::game::room;

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

unsigned int& PlayerStats::wins() {
    return this->_wins;
}

unsigned int PlayerStats::games() const {
    return this->_games;
}

unsigned int& PlayerStats::games() {
    return this->_games;
}
