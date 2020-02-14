#include <boost/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>
#include <iostream>
#include <unordered_map>

#include "MapWrapper.h"

////// In-Game State

using MapValue = MapWrapper<std::string, boost::recursive_variant_>;
using ValueList = std::vector<boost::recursive_variant_>;

// todo: duplicate DS, share this variant with the Rule module

using EnvValue = boost::make_recursive_variant<
    MapValue,
    ValueList,
    std::string,
    int,
    bool>::type;


////// Game Settings

enum DataKind {
    INTEGER,
    STRING,
    BOOLEAN,
    QUESTION_ANSWER,
    MULTIPLE_CHOICE
};

struct MapSetupValue {
    DataKind kind;
    std::string prompt;
};

using SetupValue = boost::make_recursive_variant<
    MapSetupValue,
    std::string,
    int,
    bool>::type;


////// Player and Audience States

struct GamePlayer {
    std::string name;
};


struct GameAudience {
    std::string name;
};


///////////////////////////////////


using Environment = MapWrapper<std::string, EnvValue>;
using Setup = MapWrapper<std::string, SetupValue>;


struct GameState {
    Environment constants;
    Environment variables;
    Environment perPlayer;
    Environment perAudience;
    Setup setup;

    // player exclusive data contained here or in userStates?
    // A user might have game-agnostic data
    std::vector<GamePlayer> players;
    std::vector<GameAudience> audience;
};