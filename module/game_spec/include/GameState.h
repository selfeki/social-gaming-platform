#pragma once

#include "Expression.h"
#include "ExpressionPtr.h"
#include "MapWrapper.h"
#include "Rule.h"
#include "Timer.h"

#include <optional>
#include <stack>
#include <string_view>


namespace gameSpecification {

struct RuleState {

    RuleState(rule::Rule* _rule) : rule(_rule), 
        finished(false), needsInput(false), nestedRulesInProgess(false),
        elemListIndex(0) {
        this->parent = rule->parent;
        this->next = &(*rule->next);
        this->nested = &(*rule->nested);
    }

    rule::Rule* rule;
    rule::Rule* parent;
    rule::Rule* next;
    rule::Rule* nested;
    
    //put any stateful things required to execute a rule in this structure.
    //TODO: may have to implement inheritance on this structure, for every rule, (visitor pattern appropriate here?)
    bool finished;
    bool needsInput;
    bool nestedRulesInProgess;

    //foreach rule only
    int elemListIndex;

    //etc... *sigh*

};


struct GameMessage {
    ExpMap targetUser;
    std::string content;
};

enum InputType{ CHOICE, TEXT, VOTE };

struct InputRequest {
    InputType type;
    ExpMap targetUser;
    std::string prompt;
    std::vector<std::string> choices;
    Expression* resultPtr;
    std::optional<int> timeout;
};


using RuleStateStack = std::stack<RuleState>;

struct GameState {
    ExpMap constants;
    ExpMap variables;
    ExpMap perPlayer;
    ExpMap perAudience;

    // keeps track of local variables
    std::vector<ExpMap> context;
    RuleStateStack ruleStateStack;
    std::vector<TimerObject> timerList;

    // Design: should these 2 be combined?
    // stores input requests to be delivered to users
    std::vector<InputRequest> inputRequests;
    std::vector<GameMessage>  messageQueue;

    void 
    enterScope() {
        context.emplace_back();
    }

    void 
    exitScope() {
        context.pop_back();
    }

    void
    enqueueInputRequest(const InputRequest& request) {
        // todo: check if name exists in the game
        inputRequests.push_back(request);
    }

    void
    enqueueMessage(const ExpMap& userData, const std::string message) {
        // todo: check if name exists in the game
        messageQueue.push_back({userData, std::move(message)});
    }

    void
    enqueueGlobalMessage(const std::string message) {
        for (auto userTy : {"players", "audience"}) {
            auto users = castExp<ExpList>(variables.map[userTy]);
            for (const auto& exp : users.list) {
                auto userData = castExp<ExpMap>(exp);
                assert(userData.map.count("name") > 0);
                enqueueMessage(userData, message);
            }
        }
    }
};




}    // namespace gameSpecification

