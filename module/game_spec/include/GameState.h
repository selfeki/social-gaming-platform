#pragma once

#include "Expression.h"
#include "MapWrapper.h"

namespace gameSpecification {


// A GameState is the input and output of the DSLInterpreter

struct GamePlayer {
    std::string name;
    std::vector<std::string> messages;
};

struct GameAudience {
    std::string name;
    std::vector<std::string> messages;
};

using Environment = MapWrapper<std::string, Expression>;

struct GameState {
    Environment constants;
    Environment variables;
    Environment perPlayer;
    Environment perAudience;

    // player exclusive data contained here or in userStates?
    // A user might have game-agnostic data
    std::vector<GamePlayer> players;
    std::vector<GameAudience> audience;
};


}    // namespace gameSpecification
