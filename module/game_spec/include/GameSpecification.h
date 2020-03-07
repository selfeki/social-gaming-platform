#pragma once

#include "Expression.h"
#include "GameState.h"
#include "MapWrapper.h"
#include "Rule.h"

#include <optional>
#include <string>
#include <vector>


namespace gameSpecification {

////// Game Setup related

enum DataKind {
    INTEGER,
    STRING,
    BOOLEAN,
    QUESTION_ANSWER,
    MULTIPLE_CHOICE
};

struct CustomSetup {
    DataKind kind;
    std::string prompt;
};

// todo:
// - handle default setup overriding when user creates game instance
// - handle mandatory user provided setup
//
// Design decision:
// Should setup be part of the GameState (in constants)
// OR should setup be passed with the game state to interpreter?
//
// Note: maybe  structure of domain rep need not reflect the json?
using SetupValue = boost::variant<
    CustomSetup,
    std::string,
    int,
    bool>;

using Setup = MapWrapper<std::string, SetupValue>;

//////////


struct PlayerCount {
    int min;
    int max;
};

struct Configuration {
    std::string name;
    PlayerCount count;
    bool allowAudience;
    Setup setup;
};

struct Specification {
    Configuration configuration;
    GameState gameState;
    rule::RuleList rules;
};


}    // namespace gameSpecification