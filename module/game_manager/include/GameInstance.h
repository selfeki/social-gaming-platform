
#include "DSLInterpreter.h"
#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"


namespace gameSpecification{


using rule::InterpretVisitor;
using rule::Rule;
using rule::RuleList;


class GameInstance {
public:
    GameInstance(GameState& state, Rule* firstRule)
        : interpreter { state }
        , rule_state_stack { state.ruleStateStack }
        , ruleInd { 0 }
        , isTerminated { false } 
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
                if (next_rule != nullptr) {
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

    /*
    *Load players into game state
    * */
    void loadPlayers(const std::set<arepa::chat::MemberPtr<arepa::chat::Player>>& _players){
        
        //More player loading based things can go after/before this bit
        //iterate over _players
        ExpList users;
        for (auto const& player : _players) {
            ExpMap user;
            user.map["name"] = player->name(); //preferred this to be player's session id, but Expression needs to be changed
            //iterate over perPlayer
            for (auto const& [key,value] : interpreter.getGameState().perPlayer.map) {
                user.map[key] = value;
            }
            users.list.push_back(user);
        }
        interpreter.getGameState().variables.map["players"] = users;

    }

    /*
    *Load audience into game state
    * */
    void loadAudience(const std::set<arepa::chat::MemberPtr<arepa::chat::Spectator>>& _spectators) {
        ExpList spectators;
        for (auto const& audience : _spectators) {
            ExpMap spectator;
            for (auto const& [key, value] : interpreter.getGameState().perAudience.map) {
                spectator.map[key] = value;
            }
            spectators.list.push_back(spectator);
        }

        if (boost::get<bool>(interpreter.getGameState().variables.map["audience"])) {
            interpreter.getGameState().variables.map["audience"] = spectators;
        }
    }

    

private:
    InterpretVisitor  interpreter;
    std::stack<Rule*> ruleStack;
    RuleStateStack& rule_state_stack;

    std::size_t ruleInd;
    bool isTerminated;
};

}