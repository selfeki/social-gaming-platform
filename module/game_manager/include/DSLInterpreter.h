#pragma once

#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"

#include <stack>

namespace gameSpecification::rule {

class InterpretVisitor : public RuleVisitor {
public:
    InterpretVisitor(const GameState& gs, Rule& firstRule) {
    
        //base case: push the first rule into the rule stack
        scope.push(&firstRule);

        //load the gamestate variables into the interpreter context
        //all names must be unique, for now
        for(auto& constant : gs.constants) {
            context.map.insert({std::string_view{constant.first}, constant.second});
        }
        for(auto& variable : gs.variables) {
            context.map.insert({std::string_view{variable.first}, variable.second});
        }
        for(auto& pp : gs.perPlayer) {
            context.map.insert({std::string_view{pp.first}, pp.second});
        }
        for(auto& pa : gs.perAudience) {
            context.map.insert({std::string_view{pa.first}, pa.second});
        }

    }

    void visitImpl(rule::ForEach&);

    void visitImpl( rule::GlobalMessage&);

    void visitImpl( rule::Add&);

    void
    setGameState(const GameState&);

    std::stack<Rule*> scope;

    ExpMap context;


private:
    //GameState& state;

};

}    // namespace gameSpecification::rule