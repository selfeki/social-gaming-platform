#pragma once

#include "Expression.h"
#include "MapWrapper.h"

namespace gameSpecification {


// A GameState is the input and output of the DSLInterpreter

using GameMessage = std::string;

using uniqueName = std::string;

struct GamePlayer {
    uniqueName name;
    std::vector<GameMessage> messages;
};

struct GameAudience {
    uniqueName name;
    std::vector<GameMessage> messages;
};

using Environment = ExpMap;

struct GameState {
    Environment constants;
    Environment variables;
    Environment perPlayer;
    Environment perAudience;
    Environment context;

    // player exclusive data contained here or in userStates?
    // A user might have game-agnostic data
    std::vector<GamePlayer> players;
    std::vector<GameAudience> audience;

    void enqueueMessage(uniqueName name, GameMessage message) {
        auto it = std::find_if(players.begin(), players.end(),
            [&name](const auto& player) { return player.name == name; });
        if (it != players.end()) {
            it->messages.push_back(message);
            return;
        }
        it = std::find_if(players.begin(), players.end(),
            [&name](const auto& player) { return player.name == name; });
        if (it != players.end()) {
            it->messages.push_back(message);
        }
    }

    void enqueuePlayersMessage(GameMessage message) {
        for (const auto& player : players) {
            enqueueMessage(player.name, message);
        }
    }

    void enqueueAudienceMessage(GameMessage message) {
        for (const auto& aud : audience) {
            enqueueMessage(aud.name, message);
        }
    }

    void enqueueGlobalMessage(GameMessage message) {
        enqueuePlayersMessage(message);
        enqueueAudienceMessage(message);
    }
};


}    // namespace gameSpecification
