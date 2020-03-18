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
        ruleStack.push(&firstRule);
    }

    void visitImpl(rule::ForEach&);

    void visitImpl(rule::GlobalMessage&);

    void visitImpl(rule::Add&);

    void visitImpl(rule::InputText&);

    void visitImpl(rule::InputChoice&);

    void
    setGameState(const GameState&);

    Expression
    getValueFromContextVariables(std::vector<std::string_view> tokens);    // todo: remove

    void
    setValueOfContextVariables(std::vector<std::string_view> tokens, Expression value);    // todo: remove

    std::stack<Rule*> scope;

    ExpMap context;    //todo: use gamestate context


private:
    //GameState& state;

    std::stack<const Rule*> ruleStack;    //todo: pick one
};


}    // namespace gameSpecification::rule