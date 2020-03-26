
#include "DSLInterpreter.h"
#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"

using namespace gameSpecification;
using rule::InterpretVisitor;
using rule::Rule;
using rule::RuleList;


class GameInstance {
public:
    GameInstance(GameState& state, Rule* firstRule)
        : interpreter { state }
        , ruleInd { 0 }
        , isTerminated { false } 
        {
            ruleStack.push(firstRule);
        }

    // Interprets rules until requires user interaction or game ends.
    // It updates passed in state based upon interpretation of the current rule.
    void
    updateState() {

        while (!ruleStack.empty()) {

            auto rule = ruleStack.top();
            //auto& timerList = interpreter.getGameState().timerList;
            //to do: improve the way you access the timerList



            rule->accept(interpreter);

            if (rule->finished) {
                ruleStack.pop();
                if (rule->next) {
                    ruleStack.push(rule->next.get());
                }
            } else if (rule->nestedRulesInProgess) {
                if (rule->nested) {
                    ruleStack.push(rule->nested.get());
                }
            } else if (rule->needsInput) {
                //change something to let the game manager know
                return;
            }
        }
    }

private:
    InterpretVisitor  interpreter;
    std::stack<Rule*> ruleStack;
    
    std::size_t ruleInd;
    bool isTerminated;
};
