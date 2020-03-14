#include "DSLInterpreter.h"
#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"

using namespace gameSpecification;
using rule::InterpretVisitor;
using rule::Rule;
using rule::RuleList;

class GameInstance {
public:
    GameInstance(GameState state, RuleList& rs)
        : gameState { state }
        , interpreter { gameState }
        , rules { rs }
        , ruleInd { 0 }
        , isTerminated { false } {}

    // Interprets rules until requires user interaction or game ends.
    // It updates passed in state based upon interpretation of the current rule.
    //
    // todo: include fields in state specifying game conclusion or user input request
    // todo: include artificial rule at end of rule list representing game conclusion?
    void
    updateState() {

        while(!interpreter.scope.empty()){
            auto rule = interpreter.scope.top();
        
            rule->accept(interpreter);

            if(rule->finished) {
                interpreter.scope.pop();
            }
            else if (rule->nestedRulesInProgess) {
                //do nothing, nested rules are now on the stack
            }
            else if (rule->needsInput) {
                //change the game state to reflect this
            }

        }

    }

    GameState&
    getGameState() { return gameState; }

private:
    GameState gameState;
    InterpretVisitor interpreter;
    RuleList& rules;
    std::size_t ruleInd;
    bool isTerminated;
};
