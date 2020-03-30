
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
        , rule_state_stack { state.ruleStateStack }
        {
            //with rule state stack, the rule stack is not needed anymore
            ruleStack.push(firstRule);
        }

    // Interprets rules until requires user interaction or game ends.
    // It updates passed in state based upon interpretation of the current rule.
    void
    updateState() {

        while (!rule_state_stack.empty()) {

            auto rule_state = rule_state_stack.top();
            //auto& timerList = interpreter.state.timerList;

            (rule_state.rule)->accept(interpreter);

            if (rule_state.finished) {
                
                Rule* next_rule = rule_state.next;
                rule_state_stack.pop();
                if (next_rule != NULL) {
                    rule_state_stack.push(RuleState(next_rule)); //initialize a new rule state in the stack (this is a light weight object, so it is fine)
                }
            } else if (rule_state.nestedRulesInProgess) {
                if (rule_state.nested) {
                    rule_state_stack.push(RuleState(rule_state.nested));
                }
            } else if (rule_state.needsInput) {
                //do something
                return;
            }
        }
    }

private:
    InterpretVisitor  interpreter;
    std::stack<Rule*> ruleStack;
    RuleStateStack& rule_state_stack;

    std::size_t ruleInd;
    bool isTerminated;
};
