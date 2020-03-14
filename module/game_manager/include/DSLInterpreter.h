#pragma once

#include "arepa/game_spec/GameState.h"
#include "arepa/game_spec/Rule.h"

#include <stack>

namespace gameSpecification::rule {

std::vector<std::string_view> 
parseDotNotation(const std::string_view str);


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
        for(auto& player : gs.players) {
            context.map.insert({std::string_view{player.first}, player.second});
        }
        for(auto& spectator : gs.audience) {
            context.map.insert({std::string_view{spectator.first}, spectator.second});
        }


    }

    void visitImpl(rule::ForEach&);

    void visitImpl( rule::GlobalMessage&);

    void visitImpl( rule::Add&);

    void
    setGameState(const GameState&);

    Expression
    getValueFromContextVariables(std::vector<std::string_view> tokens);

    void
    setValueOfContextVariables(std::vector<std::string_view> tokens, Expression  value);

    std::stack<Rule*> scope;

    ExpMap context;


private:
    //GameState& state;

};

}    // namespace gameSpecification::rule