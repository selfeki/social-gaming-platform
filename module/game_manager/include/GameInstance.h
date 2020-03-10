#include "DSLInterpreter.h"
#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"

using namespace gameSpecification;
using rule::InterpretVisitor;

class GameInstance {
public:
    GameInstance(const rule::RuleList& rules);

    // Interprets rules until requires user interaction or game ends.
    // It updates passed in state based upon interpretation of the current rule.
    //
    // todo: include fields in state specifying game conclusion or user input request
    // todo: include artificial rule at end of rule list representing game conclusion?
    void
    updateState();

    GameState&
    getGameState();


private:
    //   GameState&      gameState;
    InterpretVisitor interpreter;
};
