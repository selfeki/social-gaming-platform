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

typedef std::stack<RuleState> RuleStateStack;

// A GameState is the input and output of the DSLInterpreter

using uniqueName = std::string_view;


struct GameMessage {
    uniqueName targetUser;
    std::string_view content;
};


struct InputRequest {
    uniqueName targetUser;
    // note: choices, if any, are captured in the prompt
    std::string_view prompt;
    ExpressionPtr resultPtr;
    std::optional<int> timeout;
};

// keeping this for legacy code
using Environment = ExpMap;

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
    enqueueMessage(uniqueName name, std::string_view message) {
        // todo: check if name exists in the game
        messageQueue.push_back({name, message});
    }

    void
    enqueueGlobalMessage(std::string_view message) {
        for (auto userTy : {"players", "audience"}) {
            auto users = castExp<ExpList>(variables.map[userTy]);
            for (const auto& exp : users.list) {
                auto userExpMap = castExp<ExpMap>(exp);
                assert(user.count("name") > 0);
                auto nameExp = userExpMap.map["name"];
                auto name = castExp<std::string_view>(nameExp);
                enqueueMessage(name, message);
            }
        }
    }
};




}    // namespace gameSpecification

