
#include "DSLInterpreter.h"
#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"


namespace gameSpecification{



using rule::InterpretVisitor;
using rule::Rule;
using rule::RuleList;



/*
* Outgoing message that the controller understands
**/
struct ControllerOutgoingMessage {
    std::string_view to;
    std::string message;
};

/*
* Input request that the controller understands
**/
struct ControllerInputRequest {
    std::string_view to;
    std::string prompt;
    std::vector<std::string> choices;
};


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
            user.map["name"] = *(player->name()); //preferred this to be player's session id, but Expression needs to be changed
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

    /*
    * Get messages that need to be sent to players. NOTE: removes them from the queue as well
    * */
    std::vector<ControllerOutgoingMessage> popOutgoingPlayerMessages() {
        std::vector<ControllerOutgoingMessage> outgoing_messages = {};
        std::vector<GameMessage>& msg_queue = interpreter.getGameState().messageQueue;
        outgoing_messages.reserve(msg_queue.size());

        for(auto& msg : msg_queue){            
            std::string_view name{boost::get<std::string>(msg.targetUser.map["name"])};
            outgoing_messages.push_back(ControllerOutgoingMessage{name, msg.content});
        }

        msg_queue.clear();

        return outgoing_messages;
    }

    /*
    * Get input requests from the game state. NOTE: this method does not remove them. 
    * */
    std::vector<ControllerInputRequest> getInputRequests() {
        std::vector<ControllerInputRequest> requests_to_return = {};
        std::vector<InputRequest>& requests = interpreter.getGameState().inputRequests;
        requests_to_return.reserve(requests.size());

        for(auto& request : requests) {
            std::string_view name{boost::get<std::string>(request.targetUser.map["name"])};

            requests_to_return.push_back(ControllerInputRequest{name, request.prompt, request.choices});
        }

        return requests_to_return;
    }




private:
    InterpretVisitor  interpreter;
    std::stack<Rule*> ruleStack;
    RuleStateStack& rule_state_stack;

    std::size_t ruleInd;
    bool isTerminated;
};

}