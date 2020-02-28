#pragma once

#include "MapWrapper.h"
#include "Expression.h"
#include "Rules.h"

// A GameState is the input and output of the DSLInterpreter

struct GamePlayer {
    std::string name;
};

struct GameAudience {
    std::string name;
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

    GameState(Environment constants, Environment variables, Environment perPlayer, Environment perAudience,
        std::vector<GamePlayer> players, std::vector<GameAudience> audience):
        constants{constants},
        variables{variables},
        perPlayer{perPlayer},
        perAudience{perAudience},
        players{players},
        audience{audience}
        { }
    GameState(const GameState& state):
        constants{state.constants},
        variables{state.variables},
        perPlayer{state.perPlayer},
        perAudience{state.perAudience},
        players{state.players},
        audience{state.audience}
        { }

};