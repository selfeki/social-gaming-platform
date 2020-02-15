

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
};